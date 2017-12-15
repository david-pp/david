var config = require('./config');
var log4js = require('log4js');

log4js.configure({
  appenders: [
    { type: 'console' },
    {
      type: 'file',
      filename: config.logdir + '/' + push_channel + '.log', 
      maxLogSize: 100*1024*1024,
      backups:10,
      category: 'PUSH' 
    },
  ]
});

var logger = log4js.getLogger('PUSH');
// logger.setLevel('INFO');
logger.setLevel('DEBUG'); //调试时打印更多日志

module.exports = logger;
