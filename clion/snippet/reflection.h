#ifndef TINYWORLD_TINYREFLECTION_H
#define TINYWORLD_TINYREFLECTION_H

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <type_traits>
#include <boost/any.hpp>

template <typename T>
struct Object2Bin {
    static_assert(std::is_trivial<T>::value, "Object2Bin T is not POD");

    static bool from_bin(T& obj, const std::string& bin)
    {
        if (bin.size() >= sizeof(T)) {
            std::memcpy(&obj, bin.data(), sizeof(T));
        }
        return true;
    }

    static bool to_bin(T&obj, std::string& bin)
    {
        bin.resize(sizeof(T));
        bin.assign((char*)&obj, sizeof(T));
        return true;
    }
};


template <typename T, bool ispod>
struct SerializeImpl {
    static std::string serialize( T &object) ;
};



template<typename T>
struct Serialize {
    static std::string serialize( T &object) {
        return SerializeImpl<T, std::is_trivial<T>::value>::serialize(object);
    }
};

template<>
struct Serialize<std::string> {
    static std::string serialize( std::string &o) {
        return "string-";
    }
};

template<typename T>
struct Serialize<std::vector<T>> {
    static std::string serialize( std::vector<T> &o) {
        std::ostringstream os;
        os << "[";
        for (auto i : o) {
            os << Serialize<T>::serialize(i) << ",";
        }
        os << "]";
        return os.str();
    }
};



template<typename T>
class Property {
public:
    typedef std::shared_ptr<Property> Ptr;

    Property(const std::string &name) : name_(name) {}

    const std::string &name() { return name_; }

public:
    virtual const std::type_info &type() = 0;

    virtual boost::any get(T &) = 0;

    virtual void set(T &, boost::any &) = 0;

    virtual bool from_bin(T &, const std::string &bin) = 0;

    virtual bool to_bin(T &, std::string &bin) = 0;

private:
    std::string name_;
};




template<typename T, typename MemFn>
class Property_T : public Property<T> {
public:
    Property_T(const std::string &name, MemFn fn)
            : Property<T>(name), fn_(fn) {}

    using PropRefType = typename std::result_of<MemFn(T &)>::type;
    using PropType = typename std::remove_reference<PropRefType>::type;

    virtual const std::type_info &type() final {
        return typeid(PropType);
    };

    boost::any get(T &obj) final {
        return fn_(obj);
    }

    void set(T &obj, boost::any &v) final {
        fn_(obj) = boost::any_cast<PropType>(v);
    }

    bool from_bin(T &obj, const std::string &bin) {
        return true;
    }

    bool to_bin(T &obj, std::string &bin) {

        PropType p = fn_(obj);

//        std::cout << p << std::endl;

        bin = Serialize<PropType>::serialize(p);

//        bin = "hello";
//        bin.resize(sizeof(PropType));
//        bin.assign((char*)&fn_(obj), sizeof(PropType));
        return true;
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

    typename Property<T>::Ptr getPropertyByName(const std::string &name) {
        auto it = properties_.find(name);
        if (it != properties_.end())
            return it->second;
        return typename Property<T>::Ptr();
    }

    template<typename PropType>
    PropType get(T &obj, const std::string &propname) {
        auto prop = getPropertyByName(propname);
        if (prop)
            return boost::any_cast<PropType>(prop->get(obj));
        return PropType();
    }

    template<typename PropType>
    void set(T &obj, const std::string &propname, const PropType &value) {
        auto prop = getPropertyByName(propname);
        if (prop) {
            boost::any v = value;
            prop->set(obj, v);
        }
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
                std::cout << it->name() << ":" << get<int>(it->name()) << std::endl;
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

template <typename T>
struct SerializeImpl<T, true> {
    static std::string serialize( T &object) {
        return "pod-";
    }
};

template <typename T>
struct SerializeImpl<T, false> {
    static std::string serialize( T &object) {

        Struct<T>* reflection = StructFactory::instance().classByType<T>();
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

#endif //TINYWORLD_TINYREFLECTION_H
