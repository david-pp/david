#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <boost/any.hpp>
#include <sstream>
#include "reflection.h"

///////////////////////////////////////////////


struct PodType {
    int a;
    float b;
    char c;
    char name[20];
};

struct Player {
    int id = 0;
    std::string name;
    int score = 0;

    std::vector<int> values;
    PodType pod;
};


// in .cpp

struct Register {
    Register() {
        StructFactory::instance().declare<Player>()
                .property("id", &Player::id)
                .property("name", &Player::name)
                .property("score", &Player::score)
                .property("values", &Player::values);

    }
};

Register reg;

template<typename T>
std::string serialize(const T &object) {
//    Struct<T>* reflection = StructFactory::instance().classByType<T>();
//    if (reflection){
//
//    }
    return std::string();
}



//template<>
//struct Serialize<Player> {
//    static std::string serialize(Player &p) {
//        return Serialize<int>::serialize(p.id)
//               + Serialize<std::string>::serialize(p.name)
//               + Serialize<int>::serialize(p.score)
//               + Serialize<std::vector<int>>::serialize(p.values)
//               + Serialize<PodType>::serialize(p.pod);
////        return std::to_string(p.id) + "," + p.name + "," + std::to_string(p.score);
//    }
//};

void test_b() {

    std::cout << " ------------- " << std::endl;


    Player p;
    p.id = 1024;
    p.name = "david";
    p.score = 100;
    p.values.push_back(10);
    p.values.push_back(20);
    std::cout << Serialize<Player>::serialize(p) << std::endl;

    std::vector<Player> players = {p, p, p};

    std::cout << Serialize<std::vector<Player>>::serialize(players) << std::endl;
}


void test_r() {

    Reflection<Player> p;
    p.name = "david";
    p.set<int>("id", 100);
    p.set<std::string>("name", "wang");

    p.dump();
    std::cout << p.get<int>("id") << std::endl;
    std::cout << p.get<std::string>("name") << std::endl;

    std::cout << "-----" << std::endl;

    for (auto it : Reflection<Player>::descriptor->propertyIterator()) {
        if (it->type() == typeid(int))
            std::cout << it->name() << ":" << p.get<int>(it->name()) << std::endl;
        else if (it->type() == typeid(std::string))
            std::cout << it->name() << ":" << p.get<std::string>(it->name()) << std::endl;
    }
}


// 成员变量指针
int get_player_id(Player &p, int Player::* mem) {
    return p.*mem;
}

template<typename Class, typename MemType>
MemType get_field(Class &p, MemType Class::* mem) {
    return p.*mem;
};

void test_n() {
    Player p;
    p.id = 100;
    p.name = "david";

    std::cout << __PRETTY_FUNCTION__ << std::endl;

    std::cout << get_player_id(p, &Player::id) << std::endl;
    std::cout << get_field(p, &Player::id) << std::endl;
    std::cout << get_field(p, &Player::name) << std::endl;

    int v = 10;

    std::cout << typeid(v).name() << std::endl;
    std::cout << typeid(int).name() << std::endl;

    auto get_id = std::mem_fn(&Player::id);

    using GetType = decltype(get_id);
    using IdType = std::remove_reference<decltype(get_id(std::declval<Player>()))>::type;
    using IdType2 = std::remove_reference<std::result_of<decltype(get_id)(Player & )>::type>::type;

    std::cout << get_id(p) << std::endl;
    static_assert(std::is_same<IdType, int>::value, "aaaaaaaa");
    static_assert(std::is_same<IdType, IdType2>::value, "bbbbbbbb");
}


struct Npc {
    uint32_t id = 4729;
    std::string name = "david";
    uint16_t category = 10;
};

template<>
struct Object2Bin<Npc> {
    static bool from_bin(Npc &obj, const std::string &bin) {

        return true;
    }

    static bool to_bin(Npc &obj, std::string &bin) {
        std::ostringstream os;
        os << obj.id << "," << obj.name << "," << obj.category;
        bin = os.str();
        return true;
    }
};


int main() {
    test_r();
    test_n();
    test_b();
    return 0;
}
