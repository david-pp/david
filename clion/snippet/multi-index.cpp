#include <iostream>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/any.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>

using MainKey      = uint64_t;
using SecondaryKey = uint64_t;
using Data         = std::string;

struct RecordKey {
    MainKey mainKey;
    SecondaryKey secondaryKey;

    RecordKey(const MainKey mainKey, SecondaryKey secondaryKey) :
            mainKey(mainKey),
            secondaryKey(secondaryKey) {}
};

struct Record : public RecordKey {
    Data data;

    Record(const MainKey mainKey = 0, const SecondaryKey secondaryKey = 0, const Data &data = 0) :
            RecordKey(mainKey, secondaryKey),
            data(data) {}

    friend std::ostream &operator<<(std::ostream &os, Record const &r) {
        return os << " Record[" << r.mainKey << ", " << r.secondaryKey << ", " << r.data << "]";
    }
};

struct MainKeyTag {
};
struct SecondaryKeyTag {
};
struct CompositeKeyTag {
};

using boost::multi_index_container;
using namespace boost::multi_index;

typedef boost::multi_index_container<
        Record,
        indexed_by<
                ordered_non_unique<tag<CompositeKeyTag>,
                        composite_key<Record,
                                member<RecordKey, MainKey, &RecordKey::mainKey>,
                                member<RecordKey, SecondaryKey, &RecordKey::secondaryKey>
                        > > > > RecordContainer;


int test_2() {
    RecordContainer records;
    records.insert(Record(10, 20, "12"));
    records.insert(Record(10, 30, "13"));
    records.insert(Record(10, 30, "13 - need not be unique!"));
    records.insert(Record(30, 40, "34"));
    records.insert(Record(30, 50, "35"));
    records.insert(Record(50, 60, "56"));
    records.insert(Record(50, 70, "57"));
    records.insert(Record(70, 80, "78"));

    std::cout << "\nAll records:\n----------------------------------------------------------------------\n";
    for (auto const &r : records)
        std::cout << r << "\n";

    {
        std::cout
                << "\nAll records with (main) == (10):\n----------------------------------------------------------------------\n";
        auto &index = records.get<0>();
        auto range = index.equal_range(10);
        for (auto const &r : boost::make_iterator_range(range.first, range.second))
            std::cout << r << "\n";
    }

    {
        std::cout
                << "\nAll records with (main,secondary) == (10,30):\n----------------------------------------------------------------------\n";
        auto &index = records.get<0>();
        auto range = index.equal_range(std::make_tuple(10, 30));
        for (auto const &r : boost::make_iterator_range(range.first, range.second))
            std::cout << r << "\n";
    }
}

///////////////////////////////////////////////////////////////

struct Player {
    int id = 0;
    std::string name;
    int score = 0;
};

struct MemberHolderBase {

    template<typename T>
    T get(Player& p) {
        return boost::any_cast<T>(this->_get(p));
    }


    virtual boost::any _get(Player& p) = 0;

};

template<typename MemFun>
struct MemberHolder : public MemberHolderBase {
    MemberHolder(MemFun f) : get(f) {}


    boost::any _get(Player& p)
    {
        return get(p);
    }

    MemFun get;
};

template<typename MemFunc>
MemberHolder<MemFunc> *make_mem(MemFunc get) {
    MemberHolder<MemFunc> *h = new MemberHolder<MemFunc>(get);
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return h;
}


boost::any getplayer(Player &p, const std::string &mem) {
    if ("id" == mem)
        return std::mem_fn(&Player::id)(p);
    else if ("name" == mem)
        return std::mem_fn(&Player::name)(p);
}


void test_1() {
    std::unordered_multimap<std::string, int> name2id = {
            {"david", 2},
            {"jack",  3},
            {"david", 4},
    };

    std::cout << name2id.count("david") << std::endl;

    auto range = name2id.equal_range("david");
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << it->first << ":" << it->second << std::endl;
    }

    Player p;
    p.id = 1;
    p.name = "david";

    decltype(std::mem_fn(&Player::id)) get_id = std::mem_fn(&Player::id);
    decltype(std::mem_fn(&Player::name)) get_name = std::mem_fn(&Player::name);


    get_id(p) = 100;

    std::cout << get_id(p) << std::endl;
    std::cout << get_name(p) << std::endl;

    std::cout << boost::any_cast<int>(getplayer(p, "id")) << std::endl;
    std::cout << boost::any_cast<std::string>(getplayer(p, "name")) << std::endl;


    std::cout << "------" << std::endl;


    std::unordered_map<std::string, MemberHolderBase *> fields = {
            {"id",   make_mem(std::mem_fn(&Player::id))},
            {"name", make_mem(std::mem_fn(&Player::name))},
    };

    auto getid = make_mem(std::mem_fn(&Player::id));

    std::cout << getid->get(p) << std::endl;
    std::cout << fields["id"]->get<int>(p) << std::endl;
    std::cout << fields["name"]->get<std::string>(p) << std::endl;
}


//////////////////////////////////////////////////

struct by_id {
};
struct by_name {
};

struct Employee {
    int id;
    std::string name;

    Employee(int id, const std::string &name) : id(id), name(name) {}

    bool operator<(const Employee &e) const { return id < e.id; }
};

// define a multiply indexed set with indices by id and name
using EmployeeSet = multi_index_container<
        Employee,
        indexed_by<
                // sort by employee::operator<
                ordered_unique<tag<by_id>, identity<Employee> >,
                // sort by less<string> on name
                ordered_non_unique<tag<by_name>, member<Employee, std::string, &Employee::name>
                > >
>;


void test_4() {
    EmployeeSet employees;
    employees.insert(Employee(1, "david"));
    employees.insert(Employee(2, "jack"));
    employees.insert(Employee(3, "david"));

    // sort by name
    for (auto it = employees.get<1>().begin(); it != employees.get<1>().end(); ++it) {
        std::cout << it->id << ":" << it->name << std::endl;

        if (it->name != "david") {
//            Employee e = *it;
//            e.name = "david";
//            employees.get<1>().replace(it, e);

            employees.get<1>().modify(it, [](Employee &e) {
                e.name = "david";
            });
        }
    }

//    auto& byname = employees.get<1>();
    auto &byname = employees.get<by_name>();

    std::cout << byname.count("david") << std::endl;

//    byname.erase("david");

    auto range = byname.equal_range("david");
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << it->id << ":" << it->name << std::endl;
    }

    std::cout << employees.size() << std::endl;
}


int main() {
    test_1();
//    test_2();
//    test_4();
    return 0;
}