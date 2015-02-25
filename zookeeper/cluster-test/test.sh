#!/bin/bash

PACKS=../zookeeper-3.4.6.tar.gz

SERVERNUM=5

installpack(){
	server=$1
	serverdir=/tmp/zookeeper/server00$server
	datadir=$serverdir/data
	logdir=$serverdir/logs
	clientport=218$server
	dir=$PWD
	cp $PACKS $serverdir
	cd $serverdir
	tar xvfz zookeeper-3.4.6.tar.gz
	cd $dir

	m4 -DDATA_DIR=../data -DDATA_LOGDIR=../logs -DCLIENT_PORT=$clientport zoo.cfg.test > $serverdir/zookeeper-3.4.6/conf/zoo.cfg
	echo "$server" > $datadir/myid
}

startserver() {
	server=$1
	serverdir=/tmp/zookeeper/server00$server/zookeeper-3.4.6
	dir=$PWD
	cd $serverdir
	echo ================= $PWD
	bin/zkServer.sh start
	cd $dir
}

case $1 in
	install)
		for ((i = 1; i <= $SERVERNUM; i++)); do
			installpack $i
		done
		;;
	start)
		for ((i = 1; i <= $SERVERNUM; i++)); do
			startserver $i
		done
		;;
	*)
		echo ".. install | start"
esac
