var logger = require('../logger')
var mysql = require('../mysql')
var redis = require('../redis')

function replyQueryParamsError(res) {
	res.json({ errcode : -1 });
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
	if (!charid || !friendcharid || !chatcontent) {
		replyQueryParamsError(res);
		return;
	}

	logger.info(charid, friendcharid, chatcontent)

	res.json({ errcode : 0});
}

exports.pullchats = function(req, res) {
	var charid = parseInt(req.query.charid);
	if (!charid) {
		replyQueryParamsError(res);
		return;
	}

	res.json({ errcode : 0,
		chats : [
		{timestamp:20010, id:1, name:'测试1', type:1, content:'测试一下看看！！' },
		{timestamp:20009, id:1, name:'测试1', type:1, content:'测试一下看看！！' },
		{timestamp:20008, id:1, name:'测试1', type:1, content:'测试一下看看！！' },
		]});
}