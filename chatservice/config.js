
var config = {
	// 提供HTTP服务的端口号
	app_port : 3000, 

	// Redis的IP和端口
	redis_ip  : '192.168.122.232',
	redis_port: 6379,

	// MySQL地址
	mysql : 'mysql://david:123456@127.0.0.1/david',

	// HTTP访问日志
	accesslogfile: 'logs/access.log',

	// 功能日志
	logfile: 'logs/chatapp.log',
};

module.exports = config;