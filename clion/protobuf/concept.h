
template<typename T>
struct Serializer {

    std::string serialize(const T &object) const;

    bool deserialize(T &object, const std::string &bin) const;
};


template<template <typename T> class SerializerT = ProtoSerializer, typename T>
std::string serialize(const T &object, const SerializerT &serializer = SerializerT()) {
    return serializer.serialize(object);
};

template<template <typename T> class SerializerT = ProtoSerializer, typename T>
bool deserialize(T &object, const std::string &bin, const SerializerT &serializer = SerializerT()) const {
    return serializer.deserialize(object, bin);
}

Player p;

void usage_1() {
    std::string data = serialize(p);
    deserialize(p, data);
}

void usage_2() {
    std::string data = serialize<DynProtoSerializer>(p);
    deserialize<DynProtoSerializer>(p, data);
}

void usage_3() {
    DynProtoSerializer serializer(&GeneratedProtoMappingFactory::intance());
    std::string data = serialize(p, serializer);
    deserialize(p, serializer);
}

void usage_4() {
    std::string data = serialize<JsonSerializer>(p);
    deserialize<JsonSerializer>(p, data);
}
