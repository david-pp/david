//
// Created by wangdawei on 2018/3/14.
//

#include <iostream>
#include "tinyhttp.h"

// https://github.com/mrtazz/restclient-cpp

void test_sync() {
    RestClient::init();

    {
        RestClient::Response r = RestClient::get("http://smcdserver.ztgame.com:29997/emaysendMsg?dest_mobile=13636570925&msg_content=test112s322dsfdsf1d11d&priority=5&gametype=17&acttype=36");
        std::cout << r.body << std::endl;
    }

//    RestClient::Response r = RestClient::post("http://url.com/post", "text/json", "{\"foo\": \"bla\"}");
//    RestClient::Response r = RestClient::put("http://url.com/put", "text/json", "{\"foo\": \"bla\"}");
//    RestClient::Response r = RestClient::del("http://url.com/delete");
//    RestClient::Response r = RestClient::head("http://url.com");

    RestClient::disable();
}


TinyHttp http;

void test_async() {
    http.async_get("https://example.com/", [](TinyHttp::Response &resp) {
        std::cout << resp.body;
        std::cout << "----get in background thread----" << std::endl;
    });

    http.async_post("http://example.com/", "image/jpeg", "........", [](TinyHttp::Response &resp) {
        std::cout << resp.body;
        std::cout << "----post in background thread----" << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));
}

int main() {
    test_sync();
}


