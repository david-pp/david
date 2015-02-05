var mysql      = require('mysql');
var Connection = require('mysql/lib/Connection')
var config     = require('./config');
var logger     = require('./logger')

exports.connect = connect;
exports.reconnect = reconnect;
exports.query = query;
exports.close = close;

var connection = null;

function connect(cb) {

	connection = mysql.createConnection(config.mysql);

	connection.connect(function(err) {
		if (cb) cb(err);

		if (err)
			logger.error('[mysql] connect failed:' + err.code);
		else
			logger.info("[mysql] connect success: mysql://%s:%s@%s:%s", 
				connection.config.user,
				connection.config.password,
				connection.config.host, 
				connection.config.port);
	});
}

function reconnect() {
	if (connection) 
		connection.end();

	connect();
}

function close() {
	if (connection) connection.end(function(err){
		if (err)
			logger.error("[mysql] close errror:" + err.code);
		});
}


function query(sql, values, cb) {
	connection.query(sql, values, function(err, rows, fields) {
		if (err)
			reconnect();
		cb(err, rows);
	});
}

connect();

function test() {

	setInterval(function()
	{
		mysql_query('select * from qahosts', [], function(err, rows) {
			if (!err)
			{
				logger.debug(rows);
				logger.info(rows[0].name);
			}
		});
	
	}, 1000)

	close();
}

//test();