var request = require('request')

exports.push_by_device = push_by_device;

xiaomi_notify_id = 0

function push_by_device(device, title, content, platform, callback) {

  xiaomi_notify_id += 1;

  var formData = {
    title : title,
    description : content,
    registration_id : 'TkYIuuPIu5D3FS0jS9/Jpch4kX2+23KjvuG5pYGNZ6g=',
    restricted_package_name : 'com.ztgame.ztas',
    notify_type : 2,
    time_to_live : 3600000, 
    notify_id : xiaomi_notify_id,
  };

  var options = {
    url: 'https://api.xmpush.xiaomi.com/v3/message/regid',
    headers: {
      'Authorization': 'key=nwSa9gFZB3MIWsqrRrsu9w=='
    },
    formData: formData
  };

  request.post(options, function optionalCallback(err, response, body) {
    if (!err && response.statusCode == 200) {
        callback(err, "xiaomi: " + body)
      } else {
        callback(err, response.statusCode)
      } 
  });
}


