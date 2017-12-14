var redis = require("redis");
var sub = redis.createClient(), pub = redis.createClient();
var msg_count = 0;

sub.on("subscribe", function (channel, count) {
});

sub.on("message", function (channel, message) {
    console.log("[message] " + channel + ": " + message);
});

sub.subscribe("mychannel");