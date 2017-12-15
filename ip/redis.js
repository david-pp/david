
var redis   = require("redis");
var config  = require('./config');
var logger  = require('./logger');

var redis_cli = redis.createClient(config.redis_port, config.redis_ip);

redis_cli.on("connect", function(err) {
	logger.info("[redis]" + redis_cli.address + ' - connected');
})

redis_cli.on("error", function (err) {
    logger.error("[redis]" + redis_cli.address + " - " + err);
});

function test() {
	setInterval(function() {
		redis_cli.set('name', 'david');

		redis_cli.get('name', function(err, reply) {
				logger.info(reply);
		});
	}, 1000);
}

//test();

module.exports = redis_cli;