//
// 解析命令行
//
var process = require("process")
if (process.argv.length < 3) {
	console.log("Usage: node push.js channel");
	process.exit(0);
}

push_channel = process.argv[2]

//
// 配置文件
//
var config = require('./config');

//
// 日志
//
var logger = require('./logger')


//
// 建立与MySQL的连接
//
var mysql = require('./mysql')


//
// 信鸽推送
//
var xg = require('./push_xg')

//
// 小米推送
//
var xiaomi = require('./push_xiaomi')

//
// 华为推送
//
var huawei = require('./push_huawei')

//
// 建立与Redis的连接并订阅命令行指定的频道
//
var redis = require("redis")
var sub = redis.createClient(config.redis_port, config.redis_ip);
var msg_count = 0;

sub.on("subscribe", function (channel, count) {
});

sub.on("message", function (channel, message) {
    logger.debug("[message] " + channel + ": " + message);
    process_message(message.trim());
});

logger.info("waiting for the message from ", push_channel)

sub.subscribe(push_channel);

//
// 处理来自AppNode的消息
//
function process_message(message) {

    var chat = JSON.parse(message)
    logger.debug(chat)

    var chat_type = parseInt(chat["type"])

    // 根据类型和ID获得设备编号
    var sql = "SELECT CID,PLATFORM,PUSHTYPE FROM APP_DEVICE WHERE ";

    if (1 == chat_type) {         // 私聊
    	sql += "CHARID="+ parseInt(chat.id) + " ORDER BY UPDATETIME DESC LIMIT 1;";
    } else if (5 == chat_type ) { // 家族
    	sql += "SEPTID=" + parseInt(chat.id);
    } else if (6 == chat_type ) { // 帮会
    	sql += "UNIONID=" + parseInt(chat.id);
    }

    logger.debug(sql)

	mysql.query(sql, [], function(err, rows) {
		if (err) {
			logger.error(err);
			return;
		}

		if (rows.length == 0) {
			logger.warn(message, 'None CID Found')
			return;
		}
		
		// 推送给所有设备
		for (var i=0; i < rows.length; i++) {
			push_by_device(rows[i]["CID"], chat.title, chat.content, parseInt(rows[i]["PLATFORM"]), parseInt(rows[i]["PUSHTYPE"]), 
				function (error, body) {
					if (error)
						logger.error(message, error)
					else	
						logger.info(message, body)
				});
		}
	});
}

// 
//  PLATFORM : 1-Andriod 2-iOS
//  PUSHTYPE : 1-个推 2-小米 3-华为 4-信鸽 
//
function push_by_device(device, title, content, platform, pushtype, callback) {
	logger.debug(device, title, content, platform, pushtype)

	if (1 == platform) {  // Andriod
		if (2 == pushtype) {     // 小米
			xiaomi.push_by_device(device, title, content, platform, callback)
		}
		else if (3 == pushtype) { // 华为
			huawei.push_by_device(device, title, content, platform, callback)
		}
		else if (4 == pushtype) { // 信鸽
			xg.push_by_device(device, title, content, platform, callback)
		}
		else {                    // 小米(默认)
			xiaomi.push_by_device(device, title, content, platform, callback)
		}
	}
	else {  // iOS
		if (4 == pushtype) {      // 信鸽
			xg.push_by_device(device, title, content, platform, callback)
		}
		else {                    // 小米(默认)
			xiaomi.push_by_device(device, title, content, platform, callback)
		}
	}
}