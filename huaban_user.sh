#!/bin/bash

if [ $# -lt 1 ]; then
	echo "[usage] $0 USER(huaban.com/USER/)"
	exit 1
fi

user=$1
huaban=http://huaban.com/$user/
#huaban=http://huaban.com/future_fighter/

boards=`curl -s $huaban | grep -o '"board_id":[0123456789]*' | sort | uniq | sed 's/"board_id".*://g'`

mkdir ~/tmp/$user

for board in $boards; do
	./huaban.sh $board $user
	#echo "./huaban.sh $board $user"
done

exit 0

