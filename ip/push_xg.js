var mysql = require('./mysql')
var logger     = require('./logger')
var config = require('./config')
var util = require('util')
var crypto = require('crypto')
var querystring = require('querystring')
var request = require('request')

exports.process_message = process_message;

//
// 处理来自AppNode的消息
//
function process_message(message) {

    var chat = JSON.parse(message)
    logger.debug(chat)

    var chat_type = parseInt(chat["type"])

    // 根据类型和ID获得设备编号
    var sql = "SELECT CID,PLATFORM FROM APP_DEVICE WHERE ";

    if (1 == chat_type) {      // 私聊
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
			logger.warn("None CID Found : " + message)
			return;
		}
		
		// 推送给所有设备
		for (var i=0; i < rows.length; i++) {
			push_by_device(rows[i]["CID"], chat.title, chat.content, parseInt(rows[i]["PLATFORM"]));
		}
	});
}

function push_by_device(device, title, content, platform) {
	var url = ""
	if (1 == platform)     // andriod
		url = make_push_url("andriod", device, title, content)
 	else if (2 == platform) // ios
	 	url = make_push_url("ios", device, title, content)

	request(url, function (error, response, body) {
	    if (!error && response.statusCode == 200) {
	      	logger.info(body)
	      
	    } else {
	    	logger.error("PUSH FAILED! " + error)
	    }
  	}) 
}

function make_push_url(platform, device, title, content) {
	var cfg = config.xg[platform]
	var now = new Date();
	var timestamp = parseInt(now.getTime()/1000) 

	// TODO: title & content 转码(GB2312->UTF8) 

	var signtext = 'GETopenapi.xg.qq.com/v2/push/single_device' + 
	               'access_id=' + cfg.access_id +
	               'device_token=' + device +
	               'message={"title":"' + title + '","content":"' + content + '","n_id":0}' +
	               'message_type=1' + 
	               'timestamp=' + timestamp + cfg.secret_key;

	var md5 = crypto.createHash('md5');
	var sign = md5.update(signtext).digest('hex')

	var httpurl = 'http://' + config.xg.address + '/v2/push/single_device?' + 
	 			  'access_id=' + cfg.access_id + '&' + 
	 			  'timestamp=' + timestamp + '&' +
	 			  'device_token=' + device + '&' +
	 			  'message_type=1&' +
	 			  'message={"title":"' + querystring.escape(title) + '","content":"' + querystring.escape(content) + '","n_id":0}' + '&' +
	 			  'sign=' + sign; 

	logger.debug(signtext)
	logger.debug(httpurl)

	return httpurl
}


