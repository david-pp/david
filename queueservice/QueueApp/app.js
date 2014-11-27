
////////////////////////////////////////////////////////
//
// 登录排队查询HTTP服务器(2014/11/18 by David)
//
////////////////////////////////////////////////////////

var express = require('express');
var http = require('http');
var path = require('path');


//
// 异常处理
//
process.on('uncaughtException', function(err){
  console.error("运行错误:" + err.message);
  console.error(err.stack);
  process.exit(1);
});

//
// 配置模块(config.js)
//
var config = require('./config');

//
// 日志模块
//
var log4js = require('log4js');
log4js.configure({
  appenders: [
    { type: 'console' }, //控制台输出
    {
      type: 'file', //文件输出
      filename: config.logfile, 
      maxLogSize: 100*1024*1024,
      backups:10,
      category: 'QueueApp' 
    },
    {
      type: 'file', //文件输出
      filename: config.accesslogfile, 
      maxLogSize: 100*1024*1024,
      backups:10,
      category: 'http' 
    }
  ]
});

var logger = log4js.getLogger('QueueApp');
logger.setLevel('INFO');
//logger.setLevel('DEBUG'); 调试时打印更多日志

//
// Redis初始化
//
var redis = require("redis");
var redis_cli = redis.createClient(config.queue_redis_port, config.queue_redis_ip);
redis_cli.on("error", function (err) {
    logger.error("[redis]" + redis_cli.host + ":" + redis_cli.port + " - " + err);
});

//
// Node.js的Express框架初始化
//
var app = express();

app.configure(function(){
  app.set('port', config.app_port);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'ejs');
  app.use(express.favicon());
  //app.use(express.logger('dev'));
  app.use(log4js.connectLogger(log4js.getLogger('http'), {level:log4js.levels.INFO}));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
  });

/*
app.configure('development', function(){
  app.use(express.errorHandler());
});
*/

//
// 创建Redis键的帮助函数
//
function makeWaitingQueueKey(server)
{
  return server + ":qw";
}

function makeEnteringQueueKey(server)
{
  return server + ":qe";
}

function makeEnteringPlayerKey(server, id)
{
  return server + ":enter:" + id;
}

function makeQueryPlayerKey(server, id)
{
  return server + ":query:" + id;
}

function makeOnlineNumKey(server)
{
  return server + ":online";
}

function checkQueryAndDelete(queuename, server, id) {
  redis_cli.exists(makeQueryPlayerKey(server, id), function (err, reply2) {
    if (err) return;
    if (!reply2) {
      redis_cli.lrem(queuename, 0, id);
      logger.debug(queuename, 'deleted:', id);
    }
  }); // exists
}

//
// 清掉指定队列中已经离开的玩家
//
function refreshQueue(queuename, server) {

 redis_cli.lrange(queuename, 0, -1, function(err, reply){
    if (err) return;
    if (!reply) return;

    for (var i = 0; i < reply.length; i++) {
      var id = reply[i];
      logger.debug(server, id);
      checkQueryAndDelete(queuename, server, id);
    } // for 
  }); // wating..
}

var gServerInfo = new Object();

//
// 处理制定服务器
//
function refreshServerQueue(server) {
  var now = new Date();

  var lasttime = gServerInfo[server];
  if (lasttime &&  (lasttime + config.leave_cleartime) > now.getTime())
  {
    //logger.info(lasttime, now.getTime(), 'faild...');
    return;
  }

  //logger.info('refreshQueue...', server)

  // 清掉Wating队列里面的已经离开的玩家
  refreshQueue(makeWaitingQueueKey(server), server);

  // 清掉Entering队列里面的已经离开的玩家
  refreshQueue(makeEnteringQueueKey(server), server);

  gServerInfo[server] = now.getTime();
}

//
// 打印当前在线、排队人数
//
function printServerInfo(server) {
  redis_cli.get(makeOnlineNumKey(server), function(err, reply){
    if (err) return
    logger.info(server + "- 在线人数 : " + reply);
  });

  redis_cli.llen(makeWaitingQueueKey(server), function(err, reply){
    if (err) return
    logger.info(server + "- 排队人数 : " + reply);
  });

  redis_cli.llen(makeEnteringQueueKey(server), function(err, reply){
    if (err) return
    logger.info(server + "- 可进入人数: " + reply);
  });      
}

//
// 定时处理已经离开的玩家
//
setInterval(function()
{
  //logger.info('... timer ...');

  redis_cli.lrange("serverlist", 0, -1, function(err, reply){
    if (err) return;
    if (!reply) return;
    for (var i = 0; i < reply.length; i++) {
      printServerInfo(reply[i]);
    } // for 
  });

}, config.serverinfo_time * 1000)

//
// 客户端请求查看排队情况
//
app.get('/index.php', function(req, res){

  var server = req.query.server_name;
  var id = req.query.key;

  // 参数不完整
  if (!server || !id) {
    res.json({"IsOK":1}); 
    return;
  }

  refreshServerQueue(server);

  redis_cli.exists(makeEnteringPlayerKey(server, id), function (err, reply) {
    // Redis连接异常
    if (err) {
      res.json({"IsOK":1} ); 
      logger.debug(server + ':' + id + ':Redis连接异常');
      return;
    }

    // 可以进入游戏了
    if (reply) {
      res.json({"IsOK":0, "status":"enter"});
      logger.debug(server + ':' + id + ':可以进入游戏');
      return;
    }

    // 不能进入游戏
    redis_cli.lrange(makeWaitingQueueKey(server), 0, -1, function(err, reply) {
      // Redis连接异常
      if (err) {
        res.json({"IsOK":1} ); 
        logger.debug(server + ':' + id + ':Redis连接异常');
        return;
      }

      //console.log(reply);

      var pos = reply.indexOf(id);

      // 已经不在排队中，可以尝试进入
      if (pos == -1)
      {
        res.json({"IsOK":0, "status":"enter"});
        logger.debug(server + ':' + id + ':已经不在排队中');
        return;
      }

      // 返回排队位置
      res.json({"IsOK":0, "status":"wait", "wait_size":pos+1, "wait_time_per_id":3.0});

      // 记录该用户的请求
      redis_cli.set(makeQueryPlayerKey(server, id), pos+1);
      redis_cli.expire(makeQueryPlayerKey(server, id), config.leave_timeout);
      logger.debug(server + ':' + id + ':排队:' + (pos+1));
      return;

    }); // lrange 
  }); // exists
});

//
// 启动排队查询服务
//
http.createServer(app).listen(app.get('port'), function(){
  console.error("排队查询服务器启动成功，端口:" + app.get('port'));
});
