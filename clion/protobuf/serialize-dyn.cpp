#include <iostream>
#include <map>
#include <string>


#include "reflection.h"

#include "player.pb.h"
#include "archive.pb.h"

#include "serialize.h"
#include "serialize-dyn.h"
#include "serialize-mapping.h"

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

//#define USE_STRUCT_FACTORY

#define SERIALIZER DynSerializer

RUN_ONCE(Player) {


    StructFactory::instance().declare<Weapon>("Weapon")
            .property<SERIALIZER>("type", &Weapon::type, 1)
            .property<SERIALIZER>("name", &Weapon::name, 2);

    StructFactory::instance().declare<Player>("Player")
            .property<SERIALIZER>("id", &Player::id, 1)
            .property<SERIALIZER>("name", &Player::name, 2)
            .property<SERIALIZER>("weapon", &Player::weapon, 3)
            .property<SERIALIZER>("weapons_map", &Player::weapons_map, 4);






}

RUN_ONCE(PlayerGenerate) {

    std::cout << "------------------ generated -----------" << std::endl;
    {
#ifdef USE_STRUCT_FACTORY
        ProtoMappingFactory::instance()
                .mapping<Weapon>("WeaponDynProto")
                .mapping<Player>("PlayerDynProto");

#else
        ProtoMappingFactory::instance().declare<Weapon>("Weapon", "WeaponDynProto")
                .property<GenSerializer>("type", &Weapon::type, 1)
                .property<GenSerializer>("name", &Weapon::name, 2);

        ProtoMappingFactory::instance().declare<Player>("Player", "PlayerDynProto")
                .property<GenSerializer>("id", &Player::id, 1)
                .property<GenSerializer>("name", &Player::name, 2)
                .property<GenSerializer>("weapon", &Player::weapon, 3)
                .property<GenSerializer>("weapons_map", &Player::weapons_map, 4);
#endif

        ProtoMappingFactory::instance().generateAllProtoDefine();
    }
}

//
// 1.演示：使用.proto生成的做映射
//
void test_1() {
    std::cout << "----------" << __PRETTY_FUNCTION__ << "----\n\n";

    std::string data;

    {
        Player p;
        p.init();
        data = dyn_serialize<GenSerializer>(p);
    }

    {
        Player p;
        dyn_deserialize<GenSerializer>(p, data);
        p.dump();
    }
}


RUN_ONCE(PlayerDynamic) {
    std::cout << "------------------ dynamic -----------" << std::endl;
    {
#ifdef USE_STRUCT_FACTORY
        DynamicProtoMappingFactory::instance()
                .mapping<Weapon>("WeaponDyn2Proto")
                .mapping<Player>("PlayerDyn2Proto");

#else
        DynamicProtoMappingFactory::instance().declare<Weapon>("Weapon", "WeaponDyn3Proto")
                .property<DynSerializer>("type", &Weapon::type, 1)
                .property<DynSerializer>("name", &Weapon::name, 2);

        DynamicProtoMappingFactory::instance().declare<Player>("Player", "PlayerDyn3Proto")
                .property<DynSerializer>("id", &Player::id, 1)
                .property<DynSerializer>("name", &Player::name, 2)
                .property<DynSerializer>("weapon", &Player::weapon, 3)
                .property<DynSerializer>("weapons_map", &Player::weapons_map, 4);
#endif

//
//    DynamicProtoMappingFactory::instance().createProtoByType<Player>();
//    DynamicProtoMappingFactory::instance().createProtoByType<Weapon>();

        // 代码创建proto描述符
        DynamicProtoMappingFactory::instance().createAllProto();

        // 输出描述符
        DynamicProtoMappingFactory::instance().generateAllProtoDefine();
    }
}


//
// 2. 演示：动态创建Descriptor
//
void test_2() {

    std::cout << "----------" << __PRETTY_FUNCTION__ << "----\n\n";

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

RUN_ONCE(PlayerImported) {
    std::cout << "------------------ imported -----------" << std::endl;
    {
#ifdef USE_STRUCT_FACTORY
        ImportProtoMappingFactory::instance()
                .mapping<Weapon>("WeaponDynProto")
                .mapping<Player>("PlayerDynProto");

#else
        ImportProtoMappingFactory::instance().declare<Weapon>("Weapon", "WeaponDyn2Proto")
                .property<ImportSerializer>("type", &Weapon::type, 1)
                .property<ImportSerializer>("name", &Weapon::name, 2);

        ImportProtoMappingFactory::instance().declare<Player>("Player", "PlayerDyn2Proto")
                .property<ImportSerializer>("id", &Player::id, 1)
                .property<ImportSerializer>("name", &Player::name, 2)
                .property<ImportSerializer>("weapon", &Player::weapon, 3)
                .property<ImportSerializer>("weapons_map", &Player::weapons_map, 4);
#endif

        ImportProtoMappingFactory::instance().opendDir("../../protobuf/");
        ImportProtoMappingFactory::instance().import("player.proto");
        ImportProtoMappingFactory::instance().generateAllProtoDefine();
    }
}

//
// 3. 演示：动态编译创建Descriptor
//
void test_3() {
    std::cout << "----------" << __PRETTY_FUNCTION__ << "----\n\n";

    std::string data;

    {
        Player p;
        p.init();
        data = dyn_serialize<ImportSerializer>(p);
    }

    {
        Player p;
        dyn_deserialize<ImportSerializer>(p, data);
        p.dump();
    }
}

int main() {
    test_1();
    test_2();
    test_3();
}