#include <iostream>
#include <functional>
#include <unordered_map>
#include <typeinfo>
#include <boost/any.hpp>

template<typename StructT>
class FieldHolderBase {
public:
    template<typename T>
    T get(StructT &obj) {
        return boost::any_cast<T>(this->_get(obj));
    }

    template<typename T>
    void set(StructT &obj, const T &value) {
        boost::any v = value;
        this->_set(obj, v);
    }


public:
    virtual const std::type_info &type() = 0;

    virtual boost::any _get(StructT &) = 0;

    virtual void _set(StructT &, boost::any &) = 0;
};

template<typename StructT, typename MemFn>
class FieldHolder : public FieldHolderBase<StructT> {
public:
    FieldHolder(MemFn fn) : fn_(fn) {}

    virtual const std::type_info &type() {
        StructT obj;
        return typeid(decltype(fn_(obj)));
    };

    boost::any _get(StructT &obj) final {
        return fn_(obj);
    }

    void _set(StructT &obj, boost::any &v) final {
        fn_(obj) = boost::any_cast<decltype(fn_(obj))>(v);
    }

private:
    MemFn fn_;
};


template<typename StructT, typename MemFn>
FieldHolder<StructT, MemFn> *createFieldHolder(MemFn fn) {
    return new FieldHolder<StructT, MemFn>(fn);
}


template<typename StructT, typename D>
struct Reflection : public StructT {
    static D descriptor;

    template<typename T>
    T get(const std::string &fieldname) {
        return descriptor.fields_[fieldname]->get<T>(*this);
    }

    template<typename T>
    void set(const std::string &fieldname, const T &value) {
        descriptor.fields_[fieldname]->set<T>(*this, value);
    }

    void dump() {
        for (auto it = descriptor.fields_.begin(); it != descriptor.fields_.end(); ++it) {
            if (it->second->type() == typeid(int))
                std::cout << it->first << ":" << it->second->get<int>(*this) << std::endl;
            else if (it->second->type() == typeid(std::string))
                std::cout << it->first << ":" << it->second->get<std::string>(*this) << std::endl;
        }
    }
};

template<typename StructT, typename D>
D Reflection<StructT, D>::descriptor;

template<typename StructT>
struct StructDescriptor {
public:
    StructDescriptor(const std::string &cname) : name(cname) {}

    StructT *clone() { return new StructT; }

    std::string name;

    template<typename T>
    StructDescriptor<StructT> &property(const std::string &fieldname, T StructT::* prop) {
        fields_[fieldname] = createFieldHolder<StructT>(std::mem_fn(prop));
        return *this;
    }

private:
    typedef std::unordered_map<std::string, FieldHolderBase<StructT> *> FieldsMap;
    FieldsMap fields_;
};


struct Player {
    int id = 0;
    std::string name;
    int score = 0;
};

struct PlayerDescriptor {
    typedef std::unordered_map<std::string, FieldHolderBase<Player> *> FieldsMap;

    FieldsMap fields_ = {
            {"id",   createFieldHolder<Player>(std::mem_fn(&Player::id))},
            {"name", createFieldHolder<Player>(std::mem_fn(&Player::name))},
    };
};


void test_r() {
    StructDescriptor<Player> descriptor("Player");
    descriptor
            .property("id", &Player::id)
            .property("name", &Player::name);

    Reflection<Player, PlayerDescriptor> p;
    p.name = "david";
    p.set<int>("id", 100);
    p.set<std::string>("name", "wang");

    p.dump();
    std::cout << p.get<int>("id") << std::endl;
    std::cout << p.get<std::string>("name") << std::endl;

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
}

struct MemberHolderBase {

    template<typename T>
    T get(Player &p) {
        return boost::any_cast<T>(this->_get(p));
    }

    template<typename T>
    void set(Player &p, const T &value) {
        boost::any v = value;
        this->_set(p, v);
    }


    virtual boost::any _get(Player &p) = 0;

    virtual void _set(Player &p, boost::any &v) = 0;

};

template<typename MemFun>
struct MemberHolder : public MemberHolderBase {
    MemberHolder(MemFun f) : get(f) {}


    boost::any _get(Player &p) {
        return get(p);
    }

    void _set(Player &p, boost::any &v) {
        get(p) = boost::any_cast<decltype(get(p))>(v);
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
    fields["name"]->set(p, std::string("wang david"));
    std::cout << fields["name"]->get<std::string>(p) << std::endl;
}


int main() {
//    test_n();
//    test_1();
    test_r();
    return 0;
}
