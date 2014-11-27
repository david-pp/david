#!/bin/bash

REDIS_HOST=127.0.0.1
REDIS_PORT=6379
ZONENAME="风雨同舟"
URL="http://127.0.0.1:3000/index.php?server_name=${ZONENAME}&key="

DATANUM=100
HTTPNUM=100

REDIS="redis-cli -h $REDIS_HOST -p $REDIS_PORT"

initdata() {
	$REDIS del $ZONENAME:qw
	$REDIS del $ZONENAME:qe
	for ((i = 1; i <= ${DATANUM}; i++ )); do
		$REDIS rpush $ZONENAME:qw $i
		$REDIS rpush $ZONENAME:qe `expr $i + ${DATANUM}`
	done
}

httprequest() {
	for ((i = 1; i <= ${HTTPNUM}; i++ )); do
		time curl "${URL}10"
	done
}

initdata

while true ; do
	time httprequest
done;
