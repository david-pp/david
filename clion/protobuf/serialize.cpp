#include <iostream>

#include "player.pb.h"
#include "archive.pb.h"

#include "serialize.h"

//////////////////////////////////////////////////////////////////////

//
// 1. 演示：直接使用-最简单的(详见simple.cpp)
//
void test_direct_1() {

    std::cout << "----" << __PRETTY_FUNCTION__ << "-----\n\n";

    std::string data;

    // 序列化
    {
        PlayerProto p;
        p.set_id(1024);
        p.set_name("david");
        p.SerializeToString(&data);
    }

    // 反序列化
    {
        PlayerProto p;
        p.ParseFromString(data);
        std::cout << p.ShortDebugString() << std::endl;
    }
}

//
// 2. 演示：直接使用-按顺序拼装多个对象（对象类型只要满足序列化约束即可，默认支持STL和C++基本类型）
//
#include "archiver.h"
void test_direct_2() {

    std::cout << "----" << __PRETTY_FUNCTION__ << "-----\n\n";

    std::string data;

    // 序列化
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

        Archiver ar;
        ar << p << p << p;        // 支持：proto生成的类型
        ar << pvec;               // 支持：std::vector
        ar << pmap;               // 支持：std::map
        ar << vecmap;             // 支持：STL的各种组合和嵌套
        ar << ar;                 // 支持：自己? YES!

//        ar.SerializeToString(&data);
        data = ::serialize(ar);
    }

    // 反序列化
    {
        PlayerProto p1, p2, p3;
        std::vector<PlayerProto> pvec;
        std::map<uint32_t, PlayerProto> pmap;
        std::vector<std::map<uint32_t, PlayerProto> > vecmap;

        Archiver ar, ar2;
//        if (ar.ParseFromString(data)) {
        if (::deserialize(ar, data)) {
            ar >> p1 >> p2 >> p3;
            ar >> pvec;
            ar >> pmap;
            ar >> vecmap;
            ar >> ar2;
        }

        std::cout << "--- p1 ----" << std::endl;
        {
            std::cout << p1.ShortDebugString() << std::endl;
        }
        std::cout << "--- p2 ----" << std::endl;
        {
            std::cout << p2.ShortDebugString() << std::endl;
        }
        std::cout << "--- p3 ----" << std::endl;
        {
            std::cout << p3.ShortDebugString() << std::endl;
        }

        std::cout << "--- vector<P> ----" << std::endl;
        {
            for (auto &p : pvec) {
                std::cout << p.ShortDebugString() << std::endl;
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

        std::cout << "---- ar ----" << std::endl;
        {
            std::cout << ar2.DebugString() << std::endl;
        }
    }
}

//////////////////////////////////////////////////////////////////////

//
// 演示：Static Object Mapping（静态对象映射）
//
// - Proto定义：
//  message WeaponProto  {
//      optional uint32 type = 1;
//      optional string name = 2;
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
// }

// - Mapping：
//    Weapon -> WeaponProto
//    Player -> PlayerProto
//

//
// 类型定义
//
struct Weapon {
    uint32_t type = 0;
    std::string name = "";

    //
    // 侵入式序列化支持
    //
    std::string SerializeAsString() const {
        WeaponProto proto;
        proto.set_type(type);
        proto.set_name(name);
        return proto.SerializeAsString();
    }

    bool ParseFromString(const std::string &data) {
        WeaponProto proto;
        if (proto.ParseFromString(data)) {
            type = proto.type();
            name = proto.name();
            return true;
        }
        return false;
    }
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

//template<>
//class Serializer<Weapon> {
//public:
//    static std::string serialize(const Weapon &value) {
//        WeaponProto proto;
//        proto.set_type(value.type);
//        proto.set_name(value.name);
//        return proto.SerializeAsString();
//    }
//
//    static bool deserialize(Weapon &value, const std::string &bin) {
//        WeaponProto proto;
//        if (proto.ParseFromString(bin)) {
//            value.type = proto.type();
//            value.name = proto.name();
//            return true;
//        }
//        return false;
//    }
//};

//
// Player的序列化支持（通过特化）-- 非侵入式
//
template<>
class Serializer<Player> {
public:
    static std::string serialize(const Player &p) {
        PlayerProto proto;
        proto.set_id(p.id);
        proto.set_name(p.name);

        // 复杂对象
        proto.set_weapons_map(::serialize(p.weapons_map));

        return proto.SerializeAsString();
    }

    static bool deserialize(Player &p, const std::string &bin) {
        PlayerProto proto;
        if (proto.ParseFromString(bin)) {
            p.id = proto.id();
            p.name = proto.name();

            // 复杂对象
            ::deserialize(p.weapons_map, proto.weapons_map());
            return true;
        }
        return false;
    }
};

void init_player(Player &p) {
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

//
// 3.演示：静态映射
//
void test_mapping_1() {
    std::cout << "----" << __PRETTY_FUNCTION__ << "-----\n\n";

    std::string data;

    // 序列化
    {
        Player p;
        init_player(p);
        data = ::serialize(p);
    }

    // 反序列化
    {
        Player p;
        ::deserialize(p, data);
        p.dump();
    }
}

//
// 4.演示：动态映射(详见serialize-dyn.cpp)
//

int main() {
    test_direct_1();
    test_direct_2();
    test_mapping_1();
    return 0;
}