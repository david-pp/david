var config = require('./config');
var log4js = require('log4js');

log4js.configure({
  appenders: [
    { type: 'console' },
    {
      type: 'file',
      filename: config.logfile, 
      maxLogSize: 100*1024*1024,
      backups:10,
      category: 'ChatApp' 
    },
    {
      type: 'file',
      filename: config.accesslogfile, 
      maxLogSize: 100*1024*1024,
      backups:10,
      category: 'http' 
    }
  ]
});

var logger = log4js.getLogger('ChatApp');
logger.setLevel('INFO');
//logger.setLevel('DEBUG'); //调试时打印更多日志

logger.http = log4js.connectLogger(log4js.getLogger('http'), {level:log4js.levels.INFO});

module.exports = logger;
