var logger = require('../logger')
var mysql = require('../mysql')
var redis = require('../redis')

function nowSeconds() {
	var date = new Date();
	return parseInt(date.getTime()/1000);
}

function replyQueryParamsError(res) {
	res.json({ errcode : -1 });
}

function friendChatEnque(source, target, targetname, type, content) {
	var key = 'qchat:' + target;
	var value = {timestamp: nowSeconds(), id:source, name:targetname, type:type, content:content};

	logger.info(key, value);

	redis.lpush(key, JSON.stringify(value));
}

exports.friendlist = function(req, res) {
	var accid = parseInt(req.query.accid);
	var zoneid = parseInt(req.query.zoneid);
	if (!accid || !zoneid) {
		replyQueryParamsError(res);
		return;
	}
	
	res.json({ errcode : 0,
		friends: [
		{accid:100, charid:200, zoneid:300, name:'测试1', country:2, face:3, level:200},
		{accid:100, charid:200, zoneid:300, name:'测试2', country:2, face:3, level:200},
		{accid:100, charid:200, zoneid:300, name:'测试3', country:2, face:3, level:200},
		]});
};

exports.friendchat = function (req, res) {
	var charid = parseInt(req.query.charid);
	var friendcharid = parseInt(req.query.friend);
	var chatcontent = req.query.chat;

	var type = parseInt(req.query.type);
	if (!type)
		type = 0;
	
	if (!charid || !friendcharid || !chatcontent) {
		replyQueryParamsError(res);
		return;
	}

	friendChatEnque(charid, friendcharid, "朋友1", type, chatcontent);
	res.json({ errcode : 0});
}

exports.pullchats = function(req, res) {
	var charid = parseInt(req.query.charid);
	if (!charid) {
		replyQueryParamsError(res);
		return;
	}

	var result = {errcode : 0, chats: []};

	var key = 'qchat:' + charid;

	redis.lrange(key, 0, -1, function(err, reply){
		if (!err) {
			for (i in reply) {
				logger.info(reply[i]);
				result.chats.push(JSON.parse(reply[i]));
			}

			redis.del(key);
		}
		res.json(result);
	});
/*
	res.json({ errcode : 0,
		chats : [
		{timestamp:20010, id:1, name:'测试1', type:1, content:'测试一下看看！！' },
		{timestamp:20009, id:1, name:'测试1', type:1, content:'测试一下看看！！' },
		{timestamp:20008, id:1, name:'测试1', type:1, content:'测试一下看看！！' },
		]});
*/
}