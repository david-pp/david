




Response resp = http::get("http://example.com/")


http::post("http://example.com/upload", "image/jpeg", &buf)
http::post_form("http://example.com/upload", "image/jpeg", &buf)




struct Metric {

};

Influx* influx = new Influx()

influx->SetHost("http://127.0.0.1/event_influx")

influx->Write("clientcrash", Tags{
    "zone" : "风雨同舟",
    "game" : "征途2"
},  Fields{
"zone" : "风雨同舟",
"game" : "征途2"
}, timestamp);






