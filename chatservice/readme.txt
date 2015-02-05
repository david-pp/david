
curl http://127.0.0.1:3000/friendlist?accid=1\&zoneid=10

{
  "errcode": 0,
  "friends": [
    {
      "accid": 100,
      "charid": 200,
      "zoneid": 300,
      "name": "测试1",
      "country": 2,
      "face": 3,
      "level": 200
    },
    {
      "accid": 100,
      "charid": 200,
      "zoneid": 300,
      "name": "测试2",
      "country": 2,
      "face": 3,
      "level": 200
    },
    {
      "accid": 100,
      "charid": 200,
      "zoneid": 300,
      "name": "测试3",
      "country": 2,
      "face": 3,
      "level": 200
    }
  ]
}


curl http://127.0.0.1:3000/friendchat?charid=1\&friend=2\&chat=测试一下%20看OKAY不!

{
  "errcode": 0
}


curl http://127.0.0.1:3000/pullchat?charid=1

{
  "errcode": 0,
  "chats": [
    {
      "timestamp": 20010,
      "id": 1,
      "name": "测试1",
      "type": 1,
      "content": "测试一下看看！！"
    },
    {
      "timestamp": 20009,
      "id": 1,
      "name": "测试1",
      "type": 1,
      "content": "测试一下看看！！"
    },
    {
      "timestamp": 20008,
      "id": 1,
      "name": "测试1",
      "type": 1,
      "content": "测试一下看看！！"
    }
  ]
}
