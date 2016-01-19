
var express = require('express');
var http = require('http');
var path = require('path');

var config = require('./config');
var logger = require('./logger')
var mysql = require('./mysql')
var redis = require('./redis')

var friend = require('./routes/friendship')

function only4test() {
  redis.set('name', 'david');

    redis.get('name', function(err, reply) {
        logger.info(reply);
    });

  mysql.query('select * from qahosts', [], function(err, rows) {
        if (!err)
        {
          logger.debug(rows);
          logger.info(rows[0].name);
        }
      });
}

//
// 异常处理
//
process.on('uncaughtException', function(err){
  console.error("运行错误:" + err.message);
  console.error(err.stack);
  process.exit(1);
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
  app.use(logger.http);
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

app.get('/friendlist', friend.friendlist);
app.get('/friendchat', friend.friendchat);
app.get('/pullchat', friend.pullchats);

//
// 启动服务
//
http.createServer(app).listen(app.get('port'), function(){
  console.error("ChatService is Ready, Port:" + app.get('port'));
});
