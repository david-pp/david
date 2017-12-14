var express = require('express')
var fs = require('fs')
var log4js = require('log4js')
var http = require('http')
var request = require('request')
var util = require('util')
var crypto = require('crypto');
var querystring = require('querystring');

var app = express()


log4js.configure({
  appenders: [
    { type: 'console' },
    {
      type: 'file',
      filename: '/tmp/ip-service.log', 
      maxLogSize: 100*1024*1024,
      backups:10,
      category: 'ip-service' 
    }
  ]
});

var logger = log4js.getLogger('console');

//
// IP信息
// 
var ips = new Array()

//
// 加载IP数据库
//
function loadIPDatabase(filename) {
  var data = fs.readFileSync(filename);
  data.toString().split('\r\n').forEach(function(line) {
    //console.log(line)
    var values = line.split(',')
    if (values.length == 8) {
      var ip = {} 
      ip['start'] = parseInt(values[0]);
      ip['end'] = parseInt(values[1]);
      ip['start_ip'] = values[2];
      ip['end_ip'] = values[3];
      ip['country'] = values[4];
      ip['province'] = values[5];
      ip['city'] = values[6];
      ip['nettype'] = values[7];

      // console.log(ip.nettype)

      ips.push(ip)
    }
  })
}

//
// 遍历查找(低效)
//
function getIpInfo_1(myip) {
  for (var i in ips) {
    if (myip >= ips[i].start && myip <= ips[i].end)
      return ips[i]
  }
  return null
}

//
// 获得IP信息(二分查找)
//
function getIpInfo(myip) {
    var low = 0;
    var high = ips.length - 1;

    while (low <= high) {
        var mid = (low + high) >> 1;
        var start = ips[mid].start;
        var end = ips[mid].end;

        if (myip >= start && myip <= end) 
          return ips[mid];
        else if (myip < start) 
          high = mid - 1;
        else
          low = mid + 1;
    }

    return null;
}

//
// 获得网络类型
//
function getNetType(myip) {
  var ip = getIpInfo(myip);
  if (ip && ip.nettype.length)
    return ip.nettype
  else
    return "未知"
}


function getCallerIP(request) {
    var ip = request.ip
    ip = ip.split(',')[0];
    ip = ip.split(':').slice(-1); //in case the ip returned in a format: "::ffff:146.xxx.xxx.xxx"
    return ip[0];
}


// convert the ip address to a decimal
// assumes dotted decimal format for input
function ipToDecimal(ip) {
        // a not-perfect regex for checking a valid ip address
  // It checks for (1) 4 numbers between 0 and 3 digits each separated by dots (IPv4)
  // or (2) 6 numbers between 0 and 3 digits each separated by dots (IPv6)
  var ipAddressRegEx = /^(\d{0,3}\.){3}.(\d{0,3})$|^(\d{0,3}\.){5}.(\d{0,3})$/;
  var valid = ipAddressRegEx.test(ip);
  if (!valid) {
    return false;
  }
  var dots = ip.split('.');
  // make sure each value is between 0 and 255
  for (var i = 0; i < dots.length; i++) {
    var dot = dots[i];
    if (dot > 255 || dot < 0) {
      return false;
    }
  }
  if (dots.length == 4) {
    // IPv4
    return ((((((+dots[0])*256)+(+dots[1]))*256)+(+dots[2]))*256)+(+dots[3]);
  } else if (dots.length == 6) {
    // IPv6
    return ((((((((+dots[0])*256)+(+dots[1]))*256)+(+dots[2]))*256)+(+dots[3])*256)+(+dots[4])*256)+(+dots[5]);
  }
  return false;
}

app.get('/nettype', function (req, res) {
  var myip = getCallerIP(req)
  var ip = req.query.ip ? req.query.ip : myip;
  
  var ip_value = ipToDecimal(ip);
  var netinfo = getIpInfo(ip_value)
  if (netinfo) {
    var nettype = netinfo.nettype
    if (nettype.indexOf('电信') == 0)
      res.send('1')
    else if (nettype.indexOf('联通') == 0)
      res.send('2')
    else
      res.send('0')

    logger.info(ip, '-', '/nettype?ip=' + ip, nettype, nettype.province, nettype.city)
  } else {
      logger.info(ip, '-', '/nettype?ip=' + ip, '未知')
  }
})

app.get('/netinfo', function (req, res) {
  var myip = getCallerIP(req)
  var ip = req.query.ip ? req.query.ip : myip;

  var ip_value = ipToDecimal(ip);
  var netinfo = getIpInfo(ip_value)
  if (netinfo) {
    res.send(netinfo)
  }
  else {
    res.send({})
  }
  logger.info(myip, '-', '/netinfo?ip=' + ip, netinfo.start_ip + '--' + netinfo.end_ip, netinfo.province, netinfo.city, netinfo.nettype)
})

app.get('/ip', function(req, res) {
  var ip = getCallerIP(req)
  res.send(ip)
  logger.info(ip, '-', '/ip')
})



//
// influxd URL
//
var influxd_url_write = 'http://127.0.0.1:8086/write?db=zt2'


// 
// 客户端数据收集
//
// clientcrash?zone=区ID&zonename=区名&accid=账号ID&charid=角色ID&version=版本号&address=宕机地址&os=操作系统
// clientping?zone=区ID&zonename=区名&accid=账号ID&charid=角色ID&ping=Xms
app.get('/clientcrash', function(req, res) {
  var ip = getCallerIP(req)
  // ip = '222.73.62.46'
  var ip_value = ipToDecimal(ip);
  var netinfo = getIpInfo(ip_value)
  if (netinfo 
        && req.query.zone 
        && req.query.zonename 
        && req.query.accid 
        && req.query.charid 
        && req.query.version 
        && req.query.address
        && req.query.os) {
    var protocol = util.format('clientcrash,zone=%s,nettype=%s,country=%s,province=%s,city=%s,version=%s,address=%s,os=%s ip="%s",charid=%s,accid=%s'
      ,req.query.zone
      ,(netinfo.nettype.length ? netinfo.nettype : 'unkown')
      ,(netinfo.country.length ? netinfo.country : 'unkown')
      ,(netinfo.province.length ? netinfo.province : 'unkown')
      ,(netinfo.city.length ? netinfo.city : 'unkown')
      ,(req.query.version.length ? req.query.version : 'unkown')
      ,(req.query.address.length ? req.query.address : 'unkown')
      ,(req.query.os.length ? req.query.os : 'unkown')
      ,ip
      ,req.query.charid
      ,req.query.accid)

      request.post({
        headers: {},
        url:     influxd_url_write,
        body:    protocol  
      }, function(error, response, body){
        if (error)
          logger.error(error)
      });

    logger.info(protocol)
    res.send('ok')

  } else {
    res.send('invliad')
  }
})

app.get('/clientping', function(req, res) {
  var ip = getCallerIP(req)
  // ip = '222.73.62.46'
  var ip_value = ipToDecimal(ip);
  var netinfo = getIpInfo(ip_value)
  if (netinfo && req.query.zone && req.query.zonename && req.query.accid && req.query.charid && req.query.ping) {
    var protocol = util.format('clientping,zone=%s,nettype=%s,country=%s,province=%s,city=%s ping=%s,ip="%s",charid=%s,accid=%s'
      ,req.query.zone
      ,(netinfo.nettype.length ? netinfo.nettype : 'unkown')
      ,(netinfo.country.length ? netinfo.country : 'unkown')
      ,(netinfo.province.length ? netinfo.province : 'unkown')
      ,(netinfo.city.length ? netinfo.city : 'unkown')
      ,req.query.ping
      ,ip
      ,req.query.charid
      ,req.query.accid)

      request.post({
        headers: {},
        url:     influxd_url_write,
        body:    protocol  
      }, function(error, response, body){
        if (error)
          logger.error(error)
      });

    logger.info(protocol)
    res.send('ok')

  } else {
    res.send('invliad')
  }
})

function randBetween(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

//
// Patchupdate数据收集
//
// 资源更新失败：/patchfailed?zone=2048&zonename=风雨同舟&file=卡住的文件名&filesize=文件大小&timeusage=用时
// 资源更新成功：/patchok?zone=2048&zonename=风雨同舟&totalsize=此次更新的所有文件大小&timeusage=此次更新用时
//

var influxd_patch_write = 'http://127.0.0.1:8086/write?db=zt2patch'

app.get('/patchfailed', function(req, res) {
  var ip = getCallerIP(req)
  // ip = '222.73.62.46'
  var ip_value = ipToDecimal(ip);
  var netinfo = getIpInfo(ip_value)
  if (netinfo && req.query.zone && req.query.zonename && req.query.file && req.query.filesize && req.query.timeusage) {
    var protocol = util.format('patchfailed,zone=%s,nettype=%s,country=%s,province=%s,city=%s,file=%s filesize=%s,ip="%s",timeusage=%s'
      ,req.query.zone
      ,(netinfo.nettype.length ? netinfo.nettype : 'unkown')
      ,(netinfo.country.length ? netinfo.country : 'unkown')
      ,(netinfo.province.length ? netinfo.province : 'unkown')
      ,(netinfo.city.length ? netinfo.city : 'unkown')
      ,(req.query.file.length ? req.query.file : 'unkown')
      ,(req.query.filesize.length ? req.query.filesize : '0')
      ,ip
      ,(req.query.timeusage.length ? req.query.timeusage : '0'))

      request.post({
        headers: {},
        url:     influxd_patch_write,
        body:    protocol  
      }, function(error, response, body){
        if (error)
          logger.error(error)
      });

    logger.info(protocol)
    res.send('ok')

  } else {
    res.send('invliad')
  }
})

//
// 上传失败日志
//
app.post('/patchlog', function (req, res) {

  var fs = require('fs');
  var dateFormat = require('dateformat');
  
  var ip = getCallerIP(req)
  var now = new Date();
  var nowdir = dateFormat(now, "yyyymmdd");

  var dir = 'patchupdate/' + nowdir;

  if (!fs.existsSync(dir)){
    fs.mkdirSync(dir);
  }

  var stream = fs.createWriteStream(dir + '/' + ip + '.log');
  req.on('data', function(chunk){
    // console.log(chunk);
    stream.write(chunk);
  })

  fs.readdir(dir, function (err, files) {
    if (!err) {
      var index = fs.createWriteStream(dir + '/' + 'index.html');
      if (index) {
        index.write('<!doctype html>\n')
        index.write('<html>\n<head>\n')
        index.write('<title>UpdateFailed</title>\n')
        index.write('<body>')
        for (var i = 0; i < files.length; i++) {
          var url = util.format('<p><a href="/%s/%s">%s</a></p>', dir, files[i], files[i])
          index.write(url)
          index.write('\n')
        }
        index.write('</body>\n</html>')
      }
    }
  });

  res.send('OK');
});


app.get('/patchok', function(req, res) {
  var ip = getCallerIP(req)
  // ip = '222.73.62.46'
  var ip_value = ipToDecimal(ip);
  var netinfo = getIpInfo(ip_value)
  if (netinfo && req.query.zone && req.query.zonename && req.query.totalsize && req.query.timeusage) {
    var protocol = util.format('patchok,zone=%s,nettype=%s,country=%s,province=%s,city=%s totalsize=%s,ip="%s",timeusage=%s,speed=%s'
      ,req.query.zone
      ,(netinfo.nettype.length ? netinfo.nettype : 'unkown')
      ,(netinfo.country.length ? netinfo.country : 'unkown')
      ,(netinfo.province.length ? netinfo.province : 'unkown')
      ,(netinfo.city.length ? netinfo.city : 'unkown')
      ,(req.query.totalsize.length ? req.query.totalsize : 'unkown')
      ,ip
      ,(req.query.timeusage.length ? req.query.timeusage : '0')
      ,((req.query.speed && req.query.speed.length) ? req.query.speed : '0'))

      request.post({
        headers: {},
        url:     influxd_patch_write,
        body:    protocol  
      }, function(error, response, body){
        if (error)
          logger.error(error)
        // logger.error(body)
      });

    logger.info(protocol)
    res.send('ok')

  } else {
    res.send('invliad')
  }
})


function push(device, title, content) {
  var now = new Date();
  var timestamp = parseInt(now.getTime()/1000) 
  // timestamp = 1512658551
  var signtext = util.format(
    'GETopenapi.xg.qq.com/v2/push/single_deviceaccess_id=2100272154device_token=%smessage={"title":"%s","content":"%s","n_id":0}message_type=1timestamp=%d9b4bde7677a47d1881c96eae1edb1064',
    device, title, content, timestamp)

  var md5 = crypto.createHash('md5');

  var sign = md5.update(signtext).digest('hex')
  var httpurl = util.format('http://openapi.xg.qq.com/v2/push/single_device?access_id=2100272154&timestamp=%d&device_token=%s&message_type=1&message={"title":"%s","content":"%s","n_id":0}&sign=%s',
    timestamp, device, querystring.escape(title), querystring.escape(content), sign)

  console.log(signtext)
  console.log(httpurl)

  return httpurl
}

app.get('/push', function(req, res) {

  var url = push('fe24a5798144746e49ed899a7f327b688ffb33e2', "测试消息", "fuckyou！！")

  console.log(url)

  http.get(url, function(response){
    var body = '';

    response.on('data', function(d) {
      body += d;
    });

    response.on('end', function() {
      res.send(body)
    });
  })
 
  //  request(url, function (error, response, body) {
  //   if (!error && response.statusCode == 200) {
  //     res.send(body) // 打印google首页
  //   }
  // }) 
})


app.use('/patchupdate', express.static(__dirname + '/patchupdate'));

loadIPDatabase('ip.txt');
logger.info('ip database : ', ips.length);

app.listen(3000, function () {
  logger.info('listening on port 3000');
})
