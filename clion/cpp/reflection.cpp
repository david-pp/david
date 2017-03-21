#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <boost/any.hpp>
#include <sstream>
#include "reflection.h"

//
// 利用构造函数执行一些初始化代码的技巧
//
struct Register {
    Register(std::function<void()> func) {
        func();
    }
};

#define REFLECTION(structname) \
        void reg_func_##structname(); \
        Register reg_obj_##structname(reg_func_##structname); \
        void reg_func_##structname()


//
// 结构体定义
//
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
    PodType podmem;
};


//
// 结构体属性反射注册
//
REFLECTION(Player) {
    StructFactory::instance().declare<Player>()
            .property("id", &Player::id)
            .property("name", &Player::name)
            .property("score", &Player::score)
            .property("values", &Player::values)
            .property("podmem", &Player::podmem);
}


//
// 属性反射演示
//  reflection | c++filt -t
//
void test_reflection() {

    std::cout << "----------- get -----------" << std::endl;
    {
        Player p;
        p.id = 1024;
        p.name = "david";

        Struct<Player> *pr = StructFactory::instance().structByType<Player>();
        if (pr) {
            // 直接GET
            std::cout << "id   = " << pr->get<int>(p, "id") << std::endl;
            std::cout << "name = " << pr->get<std::string>(p, "name") << std::endl;

            // 迭代
            for (auto prop : pr->propertyIterator()) {
                std::cout << prop->name() << "\t : \t";
                if (prop->type() == typeid(int))
                    std::cout << pr->get<int>(p, prop->name()) << std::endl;
                else if (prop->type() == typeid(std::string))
                    std::cout << pr->get<std::string>(p, prop->name()) << std::endl;
                else
                    std::cout << prop->type().name() << std::endl;
            }
        }
    }

    std::cout << "----------- set -----------" << std::endl;
    {
        Struct<Player> *pr = StructFactory::instance().structByType<Player>();
        if (pr) {
            Player p;

            pr->set<std::string>(p, "name", "david");
            pr->set<int>(p, "id", 1024);

            std::cout << "id   = " << p.id << std::endl;
            std::cout << "name = " << p.name << std::endl;
        }
    }
}


//
// 序列化演示代码
//

// POD类型偏特化
template<typename T>
struct SerializeImpl<T, true> {
    static std::string serialize(T &object) {
        return "pod-";
    }
};

// 非POD类型偏特化
template<typename T>
struct SerializeImpl<T, false> {
    static std::string serialize(T &object) {

        Struct<T> *reflection = StructFactory::instance().structByType<T>();
        if (reflection) {
            std::string bin = "(";
            for (auto prop : reflection->propertyIterator()) {
                std::string propbin;
                prop->to_bin(object, propbin);
                bin += propbin + ",";
            }

            bin += ")";
            return bin;
        }
        return "obj-";
    }
};

// 偏特化:string
template<>
struct Serialize<std::string> {
    static std::string serialize(std::string &o) {
        return "string-";
    }
};

// 偏特化:std::vector<T>
template<typename T>
struct Serialize<std::vector<T>> {
    static std::string serialize(std::vector<T> &o) {
        std::ostringstream os;
        os << "[";
        for (auto i : o) {
            os << Serialize<T>::serialize(i) << ",";
        }
        os << "]";
        return os.str();
    }
};

// 搞一个帮助函数自动推到对象类型
template<typename T>
std::string serialize(T &obj) {
    return Serialize<T>::serialize(obj);
}

// 序列化演示
void test_serialize() {
    std::cout << "----------- serialize -----------" << std::endl;

    Player p;
    {
        p.id = 1024;
        p.name = "david";
        p.score = 100;
        p.values.push_back(10);
        p.values.push_back(20);
    }

    std::vector<Player> players = {p, p, p};
    std::vector<std::vector<std::string>> contents = {
            {"line1", "word1", "word2"},
            {"line2", "word1", "word2"}
    };

    std::cout << serialize(p) << std::endl;
    std::cout << serialize(players) << std::endl;
    std::cout << serialize(contents) << std::endl;
}

int main() {
    test_reflection();
    test_serialize();
    return 0;
}
