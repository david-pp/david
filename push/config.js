
var config = {

	// Redis的IP和端口
	redis_ip  : '127.0.0.1',
	redis_port: 6379,

	// MySQL地址
	mysql : 'mysql://david:123456@127.0.0.1/david',

	// 日志目录
	logdir: '/tmp/',

	// 信鸽配置
	xg : {
		address : 'openapi.xg.qq.com', 
		andriod : {
			access_id : '2100272154',
			access_key: 'A7VS1X914JMD',
			secret_key: '9b4bde7677a47d1881c96eae1edb1064'
		},
		ios : {
			access_id : '2200272158',
			access_key: 'I59LT2M8J7SD',
			secret_key: '02c1aac3705fc6b38d009a12dbf10a77'
		}
	}
};

module.exports = config;