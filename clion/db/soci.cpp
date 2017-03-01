#include <iostream>
#include <cstdlib>

#include "soci.h"
#include "soci-mysql.h"

//http://soci.sourceforge.net/doc/3.2/

struct Player {
    uint32_t id = 0;
    std::string name;
    uint8_t age = 0;
};

namespace soci {

    template<>
    struct type_conversion<Player> {
        typedef values base_type;

        static void from_base(const values &v, indicator ind, Player &p) {
            p.id = v.get<uint32_t>("id");
            p.name = v.get<std::string>("name");
            p.age = v.get<uint8_t>("age");
        }

        static void to_base(const Player &p, values &v, indicator &ind) {
            v.set("id", p.id);
            v.set("name", p.name);
            v.set("age", p.age);
            ind = i_ok;
        }
    };
}


void test_1() {

    try {
        soci::session sql(soci::mysql, "host=127.0.0.1 db=tinyworld user=david password='123456'");

        sql << "drop table if exists Player";
        sql << "create table Player(id int(10) unsigned, name varchar(32), age tinyint(5) unsigned, primary key(id))";

        for (int i = 0; i < 10; i++) {
            Player p;
            p.id = i + 100;
            p.name = "david";
            p.age = i;

            std::cout << "insert " << std::endl;
//            sql << "insert into Player(id, name, age) values(:id, :name, :age);", soci::use(p.id), soci::use(p.name), soci::use(p.age);
            sql << "insert into Player(id, name, age) values(:id, :name, :age)", soci::use(p);
        }

        std::cout << "--- end --" << std::endl;
    }
    catch (const soci::mysql_soci_error &e) {
        std::cout << "MySQL Error:" << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cout << "Error:" << e.what() << std::endl;
    }
}

void test_select() {
    try {
        soci::session sql(soci::mysql, "host=127.0.0.1 db=tinyworld user=david password='123456'");
        soci::row player;
        sql << "select * from player", soci::into(player);

        std::cout << player.get<uint32_t>("id") << std::endl;
        std::cout << player.get<std::string>("name") << std::endl;
        std::cout << player.get<int>("age") << std::endl;

//        for (size_t i = 0; i < players.size(); ++i) {
//            const soci::column_properties& props = players.get_properties(i);
//            std::cout << props.get_name () << std::endl;
//
//            switch (props.get_data_type()) {
//                case soci::dt_integer:
//                    std::cout << props.get_name () << ":" << players.get<int>(i) << std::endl;
//                    break;
//            }
//        }

        soci::rowset<soci::row> players = (sql.prepare << "select * from player where id>=105");
        for (auto it = players.begin(); it != players.end(); ++it) {
            std::cout << it->get<uint32_t>("id") << "\t"
                      << it->get<std::string>("name") << "\t"
                      << it->get<int>("age") << std::endl;
        }

        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    catch (const soci::mysql_soci_error &e) {
        std::cout << "MySQL Error:" << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cout << "Error:" << e.what() << std::endl;
    }
}

#include <chrono>
#include <thread>

soci::connection_pool pool(5);

void select_thread() {
    try {
        soci::session sql(pool);

        soci::rowset<soci::row> players = (sql.prepare << "select * from player where id>=105");
        for (auto it = players.begin(); it != players.end(); ++it) {
            std::cout << it->get<uint32_t>("id") << "\t"
                      << it->get<std::string>("name") << "\t"
                      << it->get<int>("age") << std::endl;
        }

        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    catch (const soci::mysql_soci_error &e) {
        std::cout << "MySQL Error:" << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cout << "Error:" << e.what() << std::endl;
    }
}

void test_multithread() {
    soci::register_factory_mysql();

    try {
        for (size_t i = 0; i < 5; ++i) {
            soci::session &sql = pool.at(i);
            sql.open("mysql://host=127.0.0.1 db=tinyworld user=david password='123456'");
        }
    }
    catch (const soci::mysql_soci_error &e) {
        std::cout << "MySQL Error:" << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cout << "Error:" << e.what() << std::endl;
    }

    std::thread w1(select_thread);
    std::thread w2(select_thread);
    std::thread w3(select_thread);


    w1.join();
    w2.join();
    w3.join();

    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

int main() {
    test_1();
    test_select();
    test_multithread();
}