

## curl命令

代码生成：
```bash
curl -i -XPOST "http://localhost:8086/write?db=example" --data-binary 'weather,location=us-midwest temperature=82' --libcurl weather.cpp
```

## libcurl

> Simple by Default, More on Demand.


- easy
- multi
- multi_socket，事件库的选择（libuv, libev, libevent, boost::asio）

VS下使用静态库时要注意：

- 设置：C++ -> 预处理：CURL_STATICLIB
- 设置：C++ -> 代码生成 -> 运行库 : /MT或者/MTd
- 库依赖（通信相关）：ws2_32.lib, wldap32.lib


## 学习资源

- Book: 《Everything Curl》
- Example：https://curl.haxx.se/libcurl/c/example.html