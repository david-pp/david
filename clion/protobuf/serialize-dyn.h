#ifndef CLION_SERIALIZE_DYN_H_H
#define CLION_SERIALIZE_DYN_H_H

#include "serialize-mapping.h"

//
// uint32_t    -> uint32
//
// std::string -> bytes
// Nested      -> bytes
// STL         -> bytes
//

using namespace google::protobuf;


template<typename T, typename ProtoMappingFactoryT>
class DynSerializerImpl {
public:
//    using ProtoMappingFactoryT = DynamicProtoMappingFactory;

    static std::string serialize(const T &object) {
        using namespace google::protobuf;

//        std::cout << __PRETTY_FUNCTION__ << std::endl;
        ProtoMapping<T> *mapping = ProtoMappingFactoryT::instance().template mappingByType<T>();
        if (mapping) {
            const Descriptor *descriptor = mapping->descriptorPool()->FindMessageTypeByName(mapping->protoName());
            const Message *prototype = mapping->messageFactory()->GetPrototype(descriptor);
            if (descriptor && prototype) {

                Message *proto = prototype->New();
                const Reflection *refl = proto->GetReflection();
                const Descriptor *desc = proto->GetDescriptor();
                if (refl && desc) {

                    try {
                        for (auto fd : mapping->struct_reflection->propertyIterator()) {
//                            std::cout << fd->name() << std::endl;
                            const FieldDescriptor *fd2 = desc->FindFieldByName(fd->name());
                            if (fd2) {
                                if (FieldDescriptor::CPPTYPE_UINT32 == fd2->cpp_type()) {
                                    uint32_t value = mapping->struct_reflection->template get<uint32_t>(object, fd->name());
                                    refl->SetUInt32(proto, fd2, value);

                                } else if (FieldDescriptor::TYPE_BYTES == fd2->type()) {
                                    auto prop = mapping->struct_reflection->propertyByName(fd->name());
                                    if (prop) {
                                        refl->SetString(proto, fd2, prop->serialize(object));
                                    }
                                }
                            }
                        }
                    } catch (std::exception &e) {
                        std::cout << e.what() << std::endl;
                    }

//                    std::cout << proto->DebugString() << std::endl;
                    return proto->SerializeAsString();
                }
            }
        }

        return "";
    }

    static bool deserialize(T &object, const std::string &data) {
        using namespace google::protobuf;
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
        ProtoMapping<T> *mapping = ProtoMappingFactoryT::instance().template mappingByType<T>();
        if (mapping) {
            const Descriptor *descriptor = mapping->descriptorPool()->FindMessageTypeByName(mapping->protoName());
            const Message *prototype = mapping->messageFactory()->GetPrototype(descriptor);
            if (descriptor && prototype) {

                Message *proto = prototype->New();
                const Reflection *refl = proto->GetReflection();
                const Descriptor *desc = proto->GetDescriptor();
                if (refl && desc && proto->ParseFromString(data)) {

                    try {
                        for (auto fd : mapping->struct_reflection->propertyIterator()) {
//                            std::cout << fd->name() << std::endl;
                            const FieldDescriptor *fd2 = desc->FindFieldByName(fd->name());
                            if (fd2) {
                                if (FieldDescriptor::CPPTYPE_UINT32 == fd2->cpp_type()) {
                                    mapping->struct_reflection->set(object, fd->name(), refl->GetUInt32(*proto, fd2));
                                } else if (FieldDescriptor::TYPE_BYTES == fd2->type()) {
                                    auto prop = mapping->struct_reflection->propertyByName(fd->name());
                                    if (prop) {
                                        prop->deserialize(object, refl->GetString(*proto, fd2));
//                                        refl->SetString(proto, fd2, prop->serialize(object));
                                    }
                                }
                            }
                        }
                    } catch (std::exception &e) {
                        std::cout << e.what() << std::endl;
                    }

//                    std::cout << proto->DebugString() << std::endl;
                    return true;
                }
            }
        }

        return false;
    }
};


//
// 序列化和反序列化的Helper函数
//

template <typename T>
using DynSerializer = DynSerializerImpl<T, DynamicProtoMappingFactory>;

template <typename T>
using GenSerializer = DynSerializerImpl<T, ProtoMappingFactory>;

template <typename T>
using ImportSerializer = DynSerializerImpl<T, ImportProtoMappingFactory>;


template<template <typename> class SerializerT = DynSerializer, typename T>
std::string dyn_serialize(const T &object) {
    return SerializerT<T>::serialize(object);
}

template<template <typename> class SerializerT = DynSerializer, typename T>
bool dyn_deserialize(T &object, const std::string &bin) {
    return SerializerT<T>::deserialize(object, bin);
}

/////////////////////////////////////////////////////////

//
// 扩展：支持std::vector<T>
//
template<typename T,  typename ProtoMappingFactoryT>
class DynSerializerImpl<std::vector<T>, ProtoMappingFactoryT> {
public:
    static std::string serialize(const std::vector<T> &objects) {
        SequenceProto proto;
        for (auto &v : objects) {
            ArchiveMemberProto *mem = proto.add_values();
            if (mem) {
                mem->set_hexdata(DynSerializerImpl<T, ProtoMappingFactoryT>::serialize(v));
            }
        }

        return proto.SerializeAsString();
    }

    static bool deserialize(std::vector<T> &objects, const std::string &bin) {
        SequenceProto proto;
        if (proto.ParseFromString(bin)) {
            for (int i = 0; i < proto.values_size(); ++i) {
                T obj;
                if (DynSerializerImpl<T, ProtoMappingFactoryT>::deserialize(obj, proto.values(i).hexdata()))
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
template<typename KeyT, typename ValueT,  typename ProtoMappingFactoryT>
class DynSerializerImpl<std::map<KeyT, ValueT>, ProtoMappingFactoryT > {
public:
    static std::string serialize(const std::map<KeyT, ValueT> &objects) {
        AssociateProto proto;
        for (auto &v : objects) {
            AssociateProto::ValueType *mem = proto.add_values();
            if (mem) {
                ArchiveMemberProto *key = mem->mutable_key();
                ArchiveMemberProto *value = mem->mutable_value();
                if (key && value) {
                    key->set_hexdata(DynSerializerImpl<KeyT, ProtoMappingFactoryT>::serialize(v.first));
                    value->set_hexdata(DynSerializerImpl<ValueT, ProtoMappingFactoryT>::serialize(v.second));
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
                if (DynSerializerImpl<KeyT, ProtoMappingFactoryT>::deserialize(key, proto.values(i).key().hexdata())
                    && DynSerializerImpl<ValueT, ProtoMappingFactoryT>::deserialize(value, proto.values(i).value().hexdata())) {
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
template< typename ProtoMappingFactoryT>
class DynSerializerImpl<uint32_t, ProtoMappingFactoryT> {
public:
    static std::string serialize(const uint32_t &value) {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
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

//
// 扩展：支持std::string
//
template< typename ProtoMappingFactoryT>
class DynSerializerImpl<std::string, ProtoMappingFactoryT> {
public:
    static std::string serialize(const std::string &value) {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
        StringProto proto;
        proto.set_value(value);
        return proto.SerializeAsString();
    }

    static bool deserialize(std::string &value, const std::string &bin) {
        StringProto proto;
        if (proto.ParseFromString(bin)) {
            value = proto.value();
            return true;
        }
        return false;
    }
};

#endif //CLION_SERIALIZE_DYN_H_H
