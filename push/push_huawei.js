var request = require('request')
var logger  = require('./logger')

exports.push_by_device = push_by_device;

huawei_access_token = ''

function push_by_device(device, title, content, platform, callback) {
  var now = new Date();
  var expires_time = new Date(now - 0 + 7*86400000)
  var timestamp = parseInt(now.getTime()/1000) 

  var payload = {
    "hps": {
      "msg": {
        "type": 3,
        "body": {
          "content": content,
          "title": title,
        },
        "action": {
          "type": 1,
          "param": {
            "intent": "#Intent;compo=com.ztgame.ztas/com.ztgame.tw.activity.LoadingActivity;"
          }
        }
      }
    }
  };

  if (huawei_access_token && huawei_access_token.length > 0) {
    var options = {
      url: 'https://api.push.hicloud.com/pushsend.do?nsp_ctx=%7b%22ver%22%3a%221%22%2c+%22appId%22%3a%22100114425%22%7d',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
      },
      form: {
        access_token : huawei_access_token,
        nsp_svc : 'openpush.message.api.send',
        nsp_ts : timestamp,
        device_token_list : JSON.stringify(["0861757036432766300000965200CN01"]),
        expire_time : expires_time.toISOString(),
        payload : JSON.stringify(payload),
      },
    };

    logger.debug(options)

    request.post(options, function optionalCallback(err, response, body) {
      if (!err && response.statusCode == 200) {
        callback(err, "huawei: " + body)
      } else {
        callback(err, response.statusCode)
      } 
    });
  }
}

function acquire_token() {
  var options = {
    url: 'https://login.vmall.com/oauth2/token',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
    },
    form: {
      grant_type : 'client_credentials',
      client_secret : 'c863103135cd5077c63cf7d2d5c7139b', 
      client_id : '100114425',
    },
  };

  request.post(options, function optionalCallback(err, response, body) {
    if (!err && response.statusCode == 200) {
      logger.info('huawei token:', body)

      var result = JSON.parse(body)
      huawei_access_token = result.access_token

      // 定时刷新token
      var expires_in = parseInt(result.expires_in)
      if (expires_in > 0)
        setTimeout(acquire_token, expires_in*1000/2)
      else
        setTimeout(acquire_token, 1000)

    } else {
      console.error('failed:', err, response.statusCode);
    } 
  });
}

acquire_token()

