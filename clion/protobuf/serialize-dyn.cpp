#include <iostream>
#include <map>
#include <string>


#include "reflection.h"

#include "player.pb.h"
#include "archive.pb.h"
#include <google/protobuf/reflection.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>

//
// uint32_t    -> uint32
//
// std::string -> bytes
// Nested      -> bytes
// STL         -> bytes
//

using namespace google::protobuf;


template<typename T>
class DynSerializer {
public:
    static const DescriptorPool *pool_;
    static MessageFactory *factory_;

    static std::string serialize(const T &object) {
        using namespace google::protobuf;

        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Struct<T> *st = StructFactory::instance().structByType<T>();
        if (st) {
            const Descriptor *descriptor = pool_->FindMessageTypeByName(
                    st->name() + "DynProto");
            const Message *prototype = factory_->GetPrototype(descriptor);
            if (descriptor && prototype) {
//                std::cout << descriptor->DebugString() << std::endl;

                Message *proto = prototype->New();
                const Reflection *refl = proto->GetReflection();
                const Descriptor *desc = proto->GetDescriptor();
                if (refl && desc) {

                    try {
                        for (auto fd : st->propertyIterator()) {
//                            std::cout << fd->name() << std::endl;
                            const FieldDescriptor *fd2 = desc->FindFieldByName(fd->name());
                            if (fd2) {
                                if (FieldDescriptor::CPPTYPE_UINT32 == fd2->cpp_type()) {
                                    uint32_t value = st->get<uint32_t>(object, fd->name());
                                    refl->SetUInt32(proto, fd2, value);

                                } else if (FieldDescriptor::TYPE_BYTES == fd2->type()) {
                                    auto prop = st->propertyByName(fd->name());
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
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Struct<T> *st = StructFactory::instance().structByType<T>();
        if (st) {
            const Descriptor *descriptor = pool_->FindMessageTypeByName(
                    st->name() + "DynProto");
            const Message *prototype = factory_->GetPrototype(descriptor);
            if (descriptor && prototype) {
//                std::cout << descriptor->DebugString() << std::endl;

                Message *proto = prototype->New();
                const Reflection *refl = proto->GetReflection();
                const Descriptor *desc = proto->GetDescriptor();
                if (refl && desc && proto->ParseFromString(data)) {

                    try {
                        for (auto fd : st->propertyIterator()) {
//                            std::cout << fd->name() << std::endl;
                            const FieldDescriptor *fd2 = desc->FindFieldByName(fd->name());
                            if (fd2) {
                                if (FieldDescriptor::CPPTYPE_UINT32 == fd2->cpp_type()) {
                                    st->set(object, fd->name(), refl->GetUInt32(*proto, fd2));
                                } else if (FieldDescriptor::TYPE_BYTES == fd2->type()) {
                                    auto prop = st->propertyByName(fd->name());
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

template<typename T>
const DescriptorPool *DynSerializer<T>::pool_ = DescriptorPool::generated_pool();
template<typename T>
MessageFactory *DynSerializer<T>::factory_ = MessageFactory::generated_factory();

//
// 序列化和反序列化的Helper函数
//
template<typename T>
std::string dyn_serialize(const T &object) {
    return DynSerializer<T>::serialize(object);
}

template<typename T>
bool dyn_deserialize(T &object, const std::string &bin) {
    return DynSerializer<T>::deserialize(object, bin);
}



/////////////////////////////////////////////////////////

//
// 扩展：支持std::vector<T>
//
template<typename T>
class DynSerializer<std::vector<T> > {
public:
    static std::string serialize(const std::vector<T> &objects) {
        SequenceProto proto;
        for (auto &v : objects) {
            ArchiveMemberProto *mem = proto.add_values();
            if (mem) {
                mem->set_hexdata(DynSerializer<T>::serialize(v));
            }
        }

        return proto.SerializeAsString();
    }

    static bool deserialize(std::vector<T> &objects, const std::string &bin) {
        SequenceProto proto;
        if (proto.ParseFromString(bin)) {
            for (int i = 0; i < proto.values_size(); ++i) {
                T obj;
                if (DynSerializer<T>::deserialize(obj, proto.values(i).hexdata()))
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
class DynSerializer<std::map<KeyT, ValueT> > {
public:
    static std::string serialize(const std::map<KeyT, ValueT> &objects) {
        AssociateProto proto;
        for (auto &v : objects) {
            AssociateProto::ValueType *mem = proto.add_values();
            if (mem) {
                ArchiveMemberProto *key = mem->mutable_key();
                ArchiveMemberProto *value = mem->mutable_value();
                if (key && value) {
                    key->set_hexdata(DynSerializer<KeyT>::serialize(v.first));
                    value->set_hexdata(DynSerializer<ValueT>::serialize(v.second));
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
                if (DynSerializer<KeyT>::deserialize(key, proto.values(i).key().hexdata())
                    && DynSerializer<ValueT>::deserialize(value, proto.values(i).value().hexdata())) {
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
class DynSerializer<uint32_t> {
public:
    static std::string serialize(const uint32_t &value) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
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
template<>
class DynSerializer<std::string> {
public:
    static std::string serialize(const std::string &value) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
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


//
// 类型定义
//
struct Weapon {
    uint32_t type = 0;
    std::string name = "";
};

struct Player {
    uint32_t id = 0;
    std::string name = "";
    Weapon weapon;
    std::map<uint32_t, std::vector<Weapon>> weapons_map;

    void init() {
        id = 1024;
        name = "david";

        weapon.type = 22;
        weapon.name = "Sword";

        Weapon w;
        w.type = 22;
        w.name = "Blade";
        weapons_map = {
                {1, {w, w, w}},
                {2, {w, w, w}},
        };
    }

    void dump() const {
        std::cout << "id   = " << id << std::endl;
        std::cout << "name = " << name << std::endl;

        std::cout << "weapon = {" << weapon.type << "," << weapon.name << "}" << std::endl;

        std::cout << "weapons_map = {" << std::endl;
        for (auto &kv : weapons_map) {
            std::cout << "\t" << kv.first << ": [";
            for (auto &p : kv.second) {
                std::cout << "{" << p.type << "," << p.name << "},";
            }
            std::cout << "]" << std::endl;
        }
        std::cout << "}" << std::endl;
    }
};

REFLECTION(Player) {

    StructFactory::instance().declare<Weapon>("Weapon")
            .property("type", &Weapon::type, 1)
            .property("name", &Weapon::name, 2);

    StructFactory::instance().declare<Player>("Player")
            .property("id", &Player::id, 1)
            .property("name", &Player::name, 2)
            .property("weapon", &Player::weapon, 3)
            .property("weapons_map", &Player::weapons_map, 4);
}

// 使用.proto生成的做映射
void test_1() {
    std::string data;

    {
        Player p;
        p.init();
        data = dyn_serialize(p);
    }

    {
        Player p;
        dyn_deserialize(p, data);
        p.dump();
    }
}

// 动态创建Descriptor

template<typename T>
struct Object2Proto {
};


DescriptorPool my_pool;
DynamicMessageFactory my_factory(&my_pool);

template<typename T>
void create() {

    std::cout << __PRETTY_FUNCTION__ << std::endl;
    Struct<T> *st = StructFactory::instance().structByType<T>();
    if (st) {
        FileDescriptorProto file_proto;
        file_proto.set_name(st->name() + ".proto");

        DescriptorProto *descriptor = file_proto.add_message_type();
        descriptor->set_name(st->name() + "DynProto");

        for (auto cpp_fd : st->propertyIterator()) {
            if (typeid(uint32_t) == cpp_fd->type()) {
                FieldDescriptorProto *fd = descriptor->add_field();
                fd->set_name(cpp_fd->name());
                fd->set_type(FieldDescriptorProto::TYPE_UINT32);
                fd->set_number(cpp_fd->id());
                fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
            } else {
                FieldDescriptorProto *fd = descriptor->add_field();
                fd->set_name(cpp_fd->name());
                fd->set_type(FieldDescriptorProto::TYPE_BYTES);
                fd->set_number(cpp_fd->id());
                fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
            }
        }

        my_pool.BuildFile(file_proto);

//        std::cout << file_proto.DebugString() << std::endl;
    }
}

void test_2() {
    create<Player>();

    const Descriptor *descriptor = my_pool.FindMessageTypeByName("PlayerDynProto");
    if (descriptor) {
        std::cout << "---- message:Player ----" << std::endl;
        std::cout << descriptor->DebugString() << std::endl;
    }

    DynSerializer<Player>::pool_ = &my_pool;
    DynSerializer<Player>::factory_ = &my_factory;

    std::string data;

    {
        Player p;
        p.init();
        data = dyn_serialize(p);
    }

    {
        Player p;
        dyn_deserialize(p, data);
        p.dump();
    }
}

int main() {
    test_1();
    test_2();
}