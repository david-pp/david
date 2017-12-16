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
// 建立与Redis的连接并订阅命令行指定的频道
//
var redis = require("redis")
var sub = redis.createClient(config.redis_port, config.redis_ip);
var msg_count = 0;

sub.on("subscribe", function (channel, count) {
});

sub.on("message", function (channel, message) {
    logger.debug("[message] " + channel + ": " + message);
    xg.process_message(message.trim());
});

logger.info("waiting for the message from ", push_channel)

sub.subscribe(push_channel);
