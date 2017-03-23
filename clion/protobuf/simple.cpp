#include <iostream>

#include "player.pb.h"

//
// 设置PlayerProto对象
//
void set(PlayerProto &p) {
    // Scalar : 1 vs N
    p.set_id(1024);
    p.set_name("david");
    p.add_quests(1);
    p.add_quests(2);

    // Nested : 1 vs N
    {
        WeaponProto *weapon = p.mutable_weapon();
        weapon->set_name("Sword");
    }

    for (int i = 0; i < 3; i++) {
        WeaponProto *weapon = p.add_weapons();
        weapon->set_type(i);
        weapon->set_name("Shield");
    }
}

//
// 获取PlayerProto对象
//
void dump(PlayerProto& p) {

//    std::cout << p.DebugString() << std::endl;
    std::cout << p.ShortDebugString() << std::endl;

    std::cout << "id      = " << p.id() << std::endl;
    std::cout << "name    = " << p.name() << std::endl;
    std::cout << "weapon  = (" << p.weapon().type() << "," << p.weapon().name() << ")" << std::endl;

    std::cout << "quests  = [";
    for (int i = 0; i < p.quests_size(); ++i) {
        std::cout << p.quests(i) << ",";
    }
    std::cout << "]" << std::endl;

    std::cout << "weapons = [";
    for (int i = 0; i < p.weapons_size(); ++i) {
        const WeaponProto &weapon = p.weapons(i);
        std::cout << weapon.ShortDebugString() << ",";
    }
    std::cout << "]" << std::endl;

}

//
// 演示简单的序列化和反序列化用法
//
void test_serialize() {
    std::cout << "-------- " << __PRETTY_FUNCTION__ << std::endl;

    PlayerProto p;
    set(p);

    std::string bin;
    p.SerializeToString(&bin);

    PlayerProto p2;
    p2.ParseFromString(bin);
    dump(p);
}

int main() {
    test_serialize();
}