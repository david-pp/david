var querystring = require('querystring')
var request = require('request')

var formData = {
	title : '标题',
	description : '测试一下！',
	registration_id : 'TkYIuuPIu5D3FS0jS9/Jpch4kX2+23KjvuG5pYGNZ6g=',
	restricted_package_name : 'com.ztgame.ztas',
	notify_type : 2,
	time_to_live : 3600000, 
	notify_id : 1,
};

var options = {
  url: 'https://api.xmpush.xiaomi.com/v3/message/regid',
  headers: {
    'Authorization': 'key=nwSa9gFZB3MIWsqrRrsu9w=='
  },
  formData: formData
};

request.post(options, function optionalCallback(err, httpResponse, body) {
  if (err) {
    return console.error(' failed:', err);
  }
  console.log('successful!  Server responded with:', body);
});
