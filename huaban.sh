#!/bin/bash

if [ $# -lt 1 ]; then
	echo "[usage] $0 BOARDID(huaban.com/boards/BOARDID)"
	exit 1
fi

board=$1
imgdir=$2

board_url=http://huaban.com/boards/$board
#board_url=http://huaban.com/boards/3427630/
#board_url=http://huaban.com/boards/394462/
image_url=http://img.hb.aicdn.com/

echo $board_url

curl -s $board_url > /tmp/huaban.html
cat /tmp/huaban.html | grep 'app.page\["board"\]' | sed 's/app.page\["board"\] =//;s/;//g' | json_xs > /tmp/huaban.tmp.1

title=`cat /tmp/huaban.html | grep '<title>.*</title>' -o | sed 's/<title>//g;s/<\/title>//g' | sed 's/[ \t]*//g'`

linenum=`cat /tmp/huaban.tmp.1 | sed -n '/"pins"/='`

keys=`cat /tmp/huaban.tmp.1 | sed "1,${linenum}d" | grep \"key\" | sed 's/.*\"key\".*://;s/\"//g'`

#echo $keys

if [ -z "$title" ] ; then
	title=$board
fi

if [ -z "$imgdir" ] ; then
	imgdir=~/tmp/$title
else
	mkdir $imgdir

	imgdir=$imgdir/$title
fi

echo $title
echo $imgdir

mkdir $imgdir

#exit 1

i=0
for key in $keys; do
	i=$(($i+1))
	url=${image_url}${key}
	curl -s $url > $imgdir/$i.jpg
	echo "[done] $url -> ~/$imgdir/$i.jpg"
done

rm /tmp/huaban.html
rm /tmp/huaban.tmp.1

exit 0

