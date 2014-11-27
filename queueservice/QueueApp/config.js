
var config = {
	// 提供HTTP服务的端口号
	app_port : 3000, 

	// 排队Redis的IP和端口
	queue_redis_ip  : '127.0.0.1',
	queue_redis_port: 6379,

	// 判断客户端离开的超时时长(秒)
	leave_timeout : 60,

	// 清理离开客户端的频率(毫秒)
	leave_cleartime: 1000, 

	// 服务器信息(人数)统计频率(秒)
	serverinfo_time : 2,

	// HTTP访问日志
	accesslogfile: 'logs/access.log',

	// 功能日志
	logfile: 'logs/queue.log',
};

module.exports = config;