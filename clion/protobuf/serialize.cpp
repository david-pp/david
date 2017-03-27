#include <iostream>

#include "player.pb.h"
#include "archive.pb.h"


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

template<typename T>
std::string serialize(const T &object) {
    return Serializer<T>::serialize(object);
}

template<typename T>
bool deserialize(T &object, const std::string &bin) {
    return Serializer<T>::deserialize(object, bin);
}


////////////////////////////////////////////

class Archiver {
public:
    template<typename T>
    Archiver &operator<<(const T &object) {
        ArchiveMemberProto *mem = ar_.add_members();
        if (mem) {
            mem->set_hexdata(Serializer<T>::serialize(object));
        }
        return *this;
    }

    template<typename T>
    Archiver &operator>>(T &object) {
        if (read_pos_ < ar_.members_size()) {
            const ArchiveMemberProto &mem = ar_.members(read_pos_);
            Serializer<T>::deserialize(object, mem.hexdata());
            read_pos_++;
        }
        return *this;
    }

private:
    ArchiveProto ar_;
    int read_pos_ = 0;
};


////////////////////////////////////////////


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
                    std::cout << v.first << std::endl;
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




////////////////////////////////////////////


//
// Archiver ar;
// ar << obj1 << obj2 << ..;
// ar >> obj1 >> obj2 >> ..;
//


//
// 直接使用
//
// std::vector<PlayerProto>
// std::map<uint32_t, PlayerProto>
//
void test_direct_1() {
    // simple.cpp
}

void test_direct_2() {
    Archiver ar;

    // serialize
    {
        PlayerProto p;
        p.set_id(1024);
        p.set_name("david");

        std::vector<PlayerProto> pvec{p, p, p};

        std::map<uint32_t, PlayerProto> pmap{
                {1, p},
                {2, p},
                {3, p}
        };

        std::vector<std::map<uint32_t, PlayerProto> > vecmap{pmap, pmap, pmap};

        ar << p << p << p;
        ar << pvec;
        ar << pmap;
        ar << vecmap;
    }

    // deserialize
    {
        PlayerProto p1, p2, p3;
        std::vector<PlayerProto> pvec;
        std::map<uint32_t, PlayerProto> pmap;
        std::vector<std::map<uint32_t, PlayerProto> > vecmap;

        ar >> p1 >> p2 >> p3;
        ar >> pvec;
        ar >> pmap;
        ar >> vecmap;

        std::cout << "--- p1 ----" << std::endl;
        {
            std::cout << p1.DebugString() << std::endl;
        }
        std::cout << "--- p2 ----" << std::endl;
        {
            std::cout << p2.DebugString() << std::endl;
        }
        std::cout << "--- p3 ----" << std::endl;
        {
            std::cout << p3.DebugString() << std::endl;
        }

        std::cout << "--- vector<P> ----" << std::endl;
        {
            for (auto &p : pvec) {
                std::cout << p.DebugString() << std::endl;
            }
        }

        std::cout << "--- map<P> ----" << std::endl;
        {
            for (auto &v : pmap) {
                std::cout << v.first << " : " << v.second.ShortDebugString() << std::endl;
            }
        }

        std::cout << "--- vector<map<P>> ----" << std::endl;
        {
            for (auto &item : vecmap) {
                for (auto &v: item)
                    std::cout << v.first << " : " << v.second.ShortDebugString() << std::endl;
                std::cout << std::endl;
            }
        }
    }
}

//
// 映射

//   message WeaponProto  {
//  optional uint32 type = 1;
//  optional string name = 2;
//  }
//
//  message PlayerProto {
//        // Scalar: 1 vs N
//        optional uint32 id = 1;
//        optional string name = 2;
//        repeated uint32 quests = 3;
//
//        // Nested: 1 vs N
//        optional WeaponProto weapon = 4;
//        repeated WeaponProto weapons = 5;
//}
//

struct Weapon {
    uint32_t type = 0;
    std::string name = "";
};

struct Player {
    uint32_t id = 0;
    std::string name = "";
    std::vector<uint32_t> quests;

    Weapon weapon;
    std::map<uint32_t, Weapon> weapons;

    std::map<uint32_t, std::vector<Weapon>> weapons_map;

    void dump() const {
        std::cout << "id   = " << id << std::endl;
        std::cout << "name = " << name << std::endl;

        std::cout << "quests = [";
        for (auto v : quests)
            std::cout << v << ",";
        std::cout << "]" << std::endl;

        std::cout << "weapon = {" << weapon.type << "," << weapon.name << "}" << std::endl;

        std::cout << "weapons_map = {" << std::endl;
        for (auto& kv : weapons_map) {
            std::cout << "\t" << kv.first << ": [";
            for (auto& p : kv.second) {
                std::cout << "{" << p.type << "," << p.name << "},";
            }
            std::cout << "]" << std::endl;
        }
        std::cout << "}" << std::endl;
    }
};

template<>
class Serializer<Weapon> {
public:
    static std::string serialize(const Weapon &value) {
        WeaponProto proto;
        proto.set_type(value.type);
        proto.set_name(value.name);
        return proto.SerializeAsString();
    }

    static bool deserialize(Weapon &value, const std::string &bin) {
        WeaponProto proto;
        if (proto.ParseFromString(bin)) {
            value.type = proto.type();
            value.name = proto.name();
            return true;
        }
        return false;
    }
};

template<>
class Serializer<Player> {
public:
    static std::string serialize(const Player &p) {
        PlayerProto proto;
        proto.set_id(p.id);
        proto.set_name(p.name);
        //...

        // 复杂对象
        proto.set_weapons_map(::serialize(p.weapons_map));

        return proto.SerializeAsString();
    }

    static bool deserialize(Player &p, const std::string &bin) {
        PlayerProto proto;
        if (proto.ParseFromString(bin)) {
            p.id = proto.id();
            p.name = proto.name();
            //...

            // 复杂对象
            ::deserialize(p.weapons_map, proto.weapons_map());
            return true;
        }
        return false;
    }
};

void init_player(Player& p) {
    p.id = 1024;
    p.name = "david";
    p.quests.push_back(1);
    p.quests.push_back(2);

    p.weapon.type = 0;
    p.weapon.name = "Sword";

    for (uint32_t i = 0; i < 3; i++) {
        p.weapons[i].type = i;
        p.weapons[i].name = "Shield";
    }

    Weapon w;
    w.type = 22;
    w.name = "Blade";
    p.weapons_map = {
            {1, {w, w, w}},
            {2, {w, w, w}},
    };

}
void test_mapping_1() {

    Archiver ar;


    // serialize
    {
        Player p;
        init_player(p);
        ar << p;
    }

    // deseriaize
    {
        Player p;
        ar >> p;

        p.dump();
    }
}



#include <google/protobuf/reflection.h>

//
// 动态构建Message：根据C++的参数类型
//
//RUN(Player) {
//    XXX::instance().declare<Player>
//                .property("id", &Player::id, 1)
//                .property("name", &Player::name, 2)
//                .property("weapons_map", &Player::weapons_map, 3)
//}
//
//  serialize(Player& p);
//

void test_mapping_2() {

}

int main() {
    test_direct_2();
    test_mapping_1();
    return 0;
}