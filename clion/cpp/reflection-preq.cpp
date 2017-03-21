#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <boost/any.hpp>
#include <sstream>

//
// 结构体定义
//
struct Player {
    int id = 0;
    std::string name;
    int score = 0;
};

//
// 成员变量指针
//
int get_player_id(Player &p, int Player::* mem) {
    return p.*mem;
}

//
// 范化的成员变量指针
//
template<typename Class, typename MemType>
MemType get_field(Class &p, MemType Class::* mem) {
    return p.*mem;
};

void test_mem() {
    Player p;
    p.id = 100;
    p.name = "david";

    std::cout << "--------- 成员变量指针--------" << std::endl;
    {
        std::cout << get_player_id(p, &Player::id) << std::endl;
    }

    std::cout << "--------- 泛化成员变量--------" << std::endl;
    {
        std::cout << get_field(p, &Player::id) << std::endl;
        std::cout << get_field(p, &Player::name) << std::endl;
    }

    std::cout << "--------- std::mem_fn --------" << std::endl;
    {
        auto get_id = std::mem_fn(&Player::id);
        auto get_name = std::mem_fn(&Player::name);

        std::cout << get_id(p) << std::endl;
        std::cout << get_name(p) << std::endl;


        //
        // type traits
        //
        using GetType = decltype(get_id);

        using IdType = std::remove_reference<decltype(get_id(std::declval<Player>()))>::type;
        using IdType2 = std::remove_reference<std::result_of<decltype(get_id)(Player & )>::type>::type;

        static_assert(std::is_same<IdType, int>::value, "aaaaaaaa");
        static_assert(std::is_same<IdType, IdType2>::value, "bbbbbbbb");
    }
}

//
// boost::any用法演示(C++17 std::any)
//
#include <boost/any.hpp>

void test_any() {

    std::cout << "--------- boost::any ---------" << std::endl;
    std::vector<boost::any> vec;
    {
        Player p;
        p.name = "david";

        vec.push_back(std::string("test"));
        vec.push_back(int(20));
        vec.push_back(p);
    }

    // 已知的情况下做转换
    try {
        std::cout << boost::any_cast<std::string>(vec[0]) << std::endl;
        std::cout << boost::any_cast<int>(vec[1]) << std::endl;
        std::cout << boost::any_cast<Player>(vec[2]).name << std::endl;

    } catch (std::exception &e) {
        std::cout << "exception:" << e.what() << std::endl;
        return;
    }

    // 迭代时需要判断类型
    for (auto v : vec) {
        if (v.type() == boost::typeindex::type_id<std::string>().type_info()) {
            std::cout << "string: " << boost::any_cast<std::string>(v) << std::endl;
        } else if (v.type() == boost::typeindex::type_id<int>().type_info()) {
            std::cout << "int   : " << boost::any_cast<int>(v) << std::endl;
        } else if (v.type() == boost::typeindex::type_id<Player>().type_info()) {
            std::cout << "player: " << boost::any_cast<Player>(v).name << std::endl;
        }
    }
}

int main() {
    test_mem();
    test_any();
    return 0;
}
