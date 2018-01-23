var request = require('request')
var logger  = require('./logger')

exports.push_by_device = push_by_device;

xiaomi_notify_id = 0

function push_by_device(device, title, content, platform, callback) {

  xiaomi_notify_id += 1;

  var key = 'key=pWR57EFBrRFgJHmg+oKQ5w=='
  var formData = {}
  if (1 == platform)      // andriod
  {
    key = 'key=nwSa9gFZB3MIWsqrRrsu9w=='

    formData = {
      title : title,
      description : content,
      registration_id : device,
      restricted_package_name : 'com.ztgame.ztas',
      notify_type : 2,
      time_to_live : 3600000, 
      notify_id : xiaomi_notify_id,
    };
  }
  else if (2 == platform) // ios
  {
    key = 'key=pWR57EFBrRFgJHmg+oKQ5w=='

    formData = {
      "aps_proper_fields.title" : title,
      "aps_proper_fields.body" : content,
      registration_id : device,
      restricted_package_name : 'com.ztgame.ztas',
      notify_type : 2,
      time_to_live : 3600000, 
      notify_id : xiaomi_notify_id,
    };
  }

  var options = {
    url: 'https://api.xmpush.xiaomi.com/v3/message/regid',
    headers: {
      'Authorization': key,
    },
    formData: formData
  };

  logger.debug(options)

  request.post(options, function optionalCallback(err, response, body) {
    if (!err && response.statusCode == 200) {
        callback(err, "xiaomi: " + body)
      } else {
        callback(err, response.statusCode)
      } 
  });
}


