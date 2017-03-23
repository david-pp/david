Protobuf的用法
--------------

##1. 使用方式


### 方式1：使用proto文件生成.h和.cc

```
simple.cpp
```

### 方式2：使用反射机制

```
reflect.cpp
```

 - 消息类型定义：
     - 静态消息类型（方式1生成的）
     
     - 动态消息类型
       - 使用xxxDescriptorProto自行构建
       - 使用Import动态编译.proto文件
       
 - 消息的操作：
    - 使用Descritor 和 Reflection进行读写
    - 动态/静态都OK
    

## 2. 用途    
    
### 用途1：消息

生产者：产生消息，填充内容，并序列化保存

消费者：读取数据，反序列化得到消息，使用消息

    
### 用途2：序列化

 - 直接使用生成的C++类型
 
``` c++
struct XXX 
{
    PlayerProto player;
}

```
 
 - 现有的类型映射到Proto类型
 
``` c++

struct Player {
    uint32_t  id;
    std::string name;
};

--> PlayerProto

```