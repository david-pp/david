#include <iostream>
#include <functional>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <boost/any.hpp>

template<typename T>
class Property {
public:
    typedef std::shared_ptr<Property> Ptr;

    Property(const std::string &name) : name_(name) {}

    virtual const std::type_info &type() = 0;

    virtual boost::any get(T &) = 0;

    virtual void set(T &, boost::any &) = 0;

    const std::string &name() { return name_; }

private:
    std::string name_;
};


template<typename T, typename MemFn>
class Property_T : public Property<T> {
public:
    Property_T(const std::string &name, MemFn fn)
            : Property<T>(name), fn_(fn) {}

    virtual const std::type_info &type() final {
        return typeid(decltype(fn_(T())));
    };

    boost::any get(T &obj) final {
        return fn_(obj);
    }

    void set(T &obj, boost::any &v) final {
        fn_(obj) = boost::any_cast<decltype(fn_(obj))>(v);
    }


private:
    MemFn fn_;
};

template<typename T, typename MemFn>
Property_T<T, MemFn> *makePropery(const std::string &name, MemFn fn) {
    return new Property_T<T, MemFn>(name, fn);
}


///////////////////////////////////////////////


template<typename T>
struct Struct {
public:
    typedef std::vector<typename Property<T>::Ptr> PropertyContainer;
    typedef std::unordered_map<std::string, typename Property<T>::Ptr> PropertyMap;

    Struct(const std::string &name) : name_(name) {}

    T *clone() { return new T; }

    template<typename PropType>
    Struct<T> &property(const std::string &name, PropType T::* prop) {
        if (!hasPropery(name)) {
            typename Property<T>::Ptr ptr(makePropery<T>(name, std::mem_fn(prop)));
            properties_[name] = ptr;
            properties_ordered_.push_back(ptr);
        }

        return *this;
    }

    bool hasPropery(const std::string &name) {
        return properties_.find(name) != properties_.end();
    }

    size_t propertyCount() { return properties_.size(); }

    PropertyContainer propertyIterator() { return properties_ordered_; }

    typename Property<T>::Ptr getPropertyByName(const std::string& name) {
        auto it = properties_.find(name);
        if (it != properties_.end())
            return it->second;
        return typename Property<T>::Ptr();
    }

private:
    std::string name_;
    PropertyContainer properties_ordered_;
    PropertyMap properties_;
};


template<typename T>
struct Reflection : public T {
    static Struct<T> *descriptor;

    template<typename PropType>
    PropType get(const std::string &propname) {
        auto prop = descriptor->getPropertyByName(propname);
        if (prop)
            return boost::any_cast<PropType>(prop->get(*this));
        return PropType();
    }

    template<typename PropType>
    void set(const std::string &propname, const PropType &value) {
        auto prop = descriptor->getPropertyByName(propname);
        if (prop) {
            boost::any v = value;
            prop->set(*this, v);
        }
    }

    void dump() {
        for (auto it : descriptor->propertyIterator()) {
            if (it->type() == typeid(int))
                std::cout << it->name()  << ":" << get<int>(it->name()) << std::endl;
            else if (it->type() == typeid(std::string))
                std::cout << it->name() << ":" << get<std::string>(it->name()) << std::endl;
        }
    }
};

template<typename T>
Struct<T> *Reflection<T>::descriptor = NULL;

struct StructFactory {
    static StructFactory &instance() {
        static StructFactory instance_;
        return instance_;
    }

    template<typename T>
    Struct<T> &declare() {
        std::string name = typeid(T).name();
        Struct<T> *desc = new Struct<T>(name);
        Reflection<T>::descriptor = desc;
        structs_[name] = desc;
        return *desc;
    }

    template<typename T>
    Struct<T> *classByType() {
        std::string name = typeid(T).name();
        if (structs_.find(name) != structs_.end())
            return boost::any_cast<Struct<T> *>(structs_[name]);
        return NULL;
    }

private:
    typedef std::unordered_map<std::string, boost::any> Structs;

    Structs structs_;
};


///////////////////////////////////////////////



struct Player {
    int id = 0;
    std::string name;
    int score = 0;
};

// in .cpp

struct Register {
    Register() {
        StructFactory::instance().declare<Player>()
                .property("id", &Player::id)
                .property("name", &Player::name);

    }
};

Register reg;


void test_r() {

    Reflection<Player> p;
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

int main() {
//    test_n();
//    test_1();
    test_r();
    return 0;
}
