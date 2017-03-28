#ifndef CLION_SERIALIZE_H
#define CLION_SERIALIZE_H

#include <string>
#include "archive.pb.h"

//
// Serializer主模板
//
//  T的约束：
//   - T 支持 SerializeAsString
//   - T 支持 ParseFromString
//
//  T的类型：
//   - 一般：只要T支持上面的约束（一般都是.proto生成的类)
//   - 扩展：特化
//

template<typename T>
class Serializer {
public:
    static std::string serialize(const T &object) {
        return object.SerializeAsString();
    }

    static bool deserialize(T &object, const std::string &bin) {
        return object.ParseFromString(bin);
    }
};

//
// 序列化和反序列化的Helper函数
//
template<typename T>
std::string serialize(const T &object) {
    return Serializer<T>::serialize(object);
}

template<typename T>
bool deserialize(T &object, const std::string &bin) {
    return Serializer<T>::deserialize(object, bin);
}


/////////////////////////////////////////////////////////

//
// 扩展：支持std::vector<T>
//
template<typename T>
class Serializer<std::vector<T> > {
public:
static std::string serialize(const std::vector<T> &objects) {
    SequenceProto proto;
    for (auto &v : objects) {
        ArchiveMemberProto *mem = proto.add_values();
        if (mem) {
            mem->set_hexdata(Serializer<T>::serialize(v));
        }
    }

    return proto.SerializeAsString();
}

static bool deserialize(std::vector<T> &objects, const std::string &bin) {
    SequenceProto proto;
    if (proto.ParseFromString(bin)) {
        for (int i = 0; i < proto.values_size(); ++i) {
            T obj;
            if (Serializer<T>::deserialize(obj, proto.values(i).hexdata()))
                objects.push_back(obj);
        }
        return true;
    }
    return false;
}
};

//
// 扩展：支持std::map<KeyT, ValueT>
//
template<typename KeyT, typename ValueT>
class Serializer<std::map<KeyT, ValueT> > {
public:
static std::string serialize(const std::map<KeyT, ValueT> &objects) {
    AssociateProto proto;
    for (auto &v : objects) {
        AssociateProto::ValueType *mem = proto.add_values();
        if (mem) {
            ArchiveMemberProto *key = mem->mutable_key();
            ArchiveMemberProto *value = mem->mutable_value();
            if (key && value) {
                key->set_hexdata(Serializer<KeyT>::serialize(v.first));
                value->set_hexdata(Serializer<ValueT>::serialize(v.second));
            }
        }
    }

    return proto.SerializeAsString();
}

static bool deserialize(std::map<KeyT, ValueT> &objects, const std::string &bin) {
    AssociateProto proto;
    if (proto.ParseFromString(bin)) {
        for (int i = 0; i < proto.values_size(); ++i) {
            KeyT key;
            ValueT value;
            if (Serializer<KeyT>::deserialize(key, proto.values(i).key().hexdata())
                && Serializer<ValueT>::deserialize(value, proto.values(i).value().hexdata())) {
                objects.insert(std::make_pair(key, value));
            }
        }
        return true;
    }
    return false;
}
};


//
// 扩展：支持uint32_t
//
template<>
class Serializer<uint32_t> {
public:
    static std::string serialize(const uint32_t &value) {
        UInt32Proto proto;
        proto.set_value(value);
        return proto.SerializeAsString();
    }

    static bool deserialize(uint32_t &value, const std::string &bin) {
        UInt32Proto proto;
        if (proto.ParseFromString(bin)) {
            value = proto.value();
            return true;
        }
        return false;
    }
};

#endif //CLION_SERIALIZE_H
