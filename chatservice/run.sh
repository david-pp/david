#!/bin/bash

NODE=node
PIDFILE=.app.pid

start()
{
	$NODE app.js
}

daemon()
{
	$NODE app.js > /dev/null &
	echo $! > $PIDFILE
}

stop()
{
	if [[ -e $PIDFILE ]]; then
		kill `cat $PIDFILE`
		rm -rf $PIDFILE
		echo "服务停止"
	else
		echo "服务已经停止了"
	fi
}

case $1 in
	start) start ;;
	daemon) daemon ;;
	stop) stop;;
	*) echo "$0 - start | daemon| stop" ;;
esac
