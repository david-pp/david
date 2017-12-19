var mysql = require('./mysql')
var logger     = require('./logger')
var config = require('./config')
var util = require('util')
var crypto = require('crypto')
var querystring = require('querystring')
var request = require('request')
var iconv = require('iconv-lite')
var encoding = require("encoding");

exports.push_by_device = push_by_device;

function push_by_device(device, title, content, platform, callback) {
	var url = ""
	if (1 == platform)     // andriod
		url = make_push_url("andriod", device, title, content)
 	else if (2 == platform) // ios
	 	url = make_push_url("ios", device, title, content)

	request(url, function (error, response, body) {
	    if (!error && response.statusCode == 200) {
	      	callback(error, "xg: " + body)
	    } else {
	    	callback(error, "")
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


