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

//
// 利用构造函数执行一些初始化代码的技巧
//
struct Register {
    Register(std::function<void()> func) {
        func();
    }
};

#define RUN_ONCE(structname, ...) \
        void reg_func_##structname(); \
        Register reg_obj_##structname(reg_func_##structname); \
        void reg_func_##structname()


template<typename T>
class Property {
public:
    typedef std::shared_ptr<Property> Ptr;

    Property(const std::string &name, uint16_t id)
            : name_(name), id_(id) {}

    const std::string &name() { return name_; }

    uint16_t number() { return id_; }

public:
    virtual const std::type_info &type() = 0;

    virtual boost::any get(const T &) = 0;

    virtual void set(T &, boost::any &) = 0;

    virtual std::string serialize(const T &) = 0;

    virtual bool deserialize(T &object, const std::string &bin) = 0;

protected:
    std::string name_;
    uint16_t id_;
};


template<typename T, typename MemFn, typename SerializerT>
class Property_T : public Property<T> {
public:
    Property_T(const std::string &name, uint16_t id, MemFn fn)
            : Property<T>(name, id), fn_(fn) {}

    using PropRefType = typename std::result_of<MemFn(T &)>::type;
    using PropType = typename std::remove_reference<PropRefType>::type;

    virtual const std::type_info &type() final {
        return typeid(PropType);
    };

    boost::any get(const T &obj) final {
        return fn_(obj);
    }

    void set(T &obj, boost::any &v) final {
        fn_(obj) = boost::any_cast<PropType>(v);
    }

    std::string serialize(const T &obj) final {
        return SerializerT::serialize(fn_(obj));
    }

    bool deserialize(T &obj, const std::string &bin) final {
        return SerializerT::deserialize(fn_(obj), bin);
    }

protected:
    MemFn fn_;
};

template<typename T, typename SerializerT, typename MemFn>
Property_T<T, MemFn, SerializerT> *makePropery(const std::string &name, uint16_t id, MemFn fn) {
    return new Property_T<T, MemFn, SerializerT>(name, id, fn);
}


struct StructBase {
    virtual void create() {}
};

template<typename T>
struct Struct : public StructBase {
public:
    typedef std::vector<typename Property<T>::Ptr> PropertyContainer;
    typedef std::unordered_map<std::string, typename Property<T>::Ptr> PropertyMap;
    typedef std::unordered_map<uint16_t, typename Property<T>::Ptr> PropertyMapByID;

    Struct(const std::string &name, uint16_t version = 0)
            : name_(name), version_(version) {}

    virtual ~Struct() {}

    T *clone() { return new T; }

    template<template <typename> class SerializerT/*=DynSerializer*/, typename PropType>
    Struct<T> &property(const std::string &name, PropType T::* prop, uint16_t id = 0) {
        if (!hasPropery(name)) {
            typename Property<T>::Ptr ptr(makePropery<T, SerializerT<PropType>>(name, id, std::mem_fn(prop)));
            properties_[name] = ptr;
            properties_ordered_.push_back(ptr);

            if (id) {
                properties_byid_[id] = ptr;
            }
        }

        return *this;
    }

    Struct<T> &version(uint16_t ver) {
        version_ = ver;
        return *this;
    }

    bool hasPropery(const std::string &name) {
        return properties_.find(name) != properties_.end();
    }

    size_t propertyCount() { return properties_.size(); }

    PropertyContainer propertyIterator() { return properties_ordered_; }

    typename Property<T>::Ptr propertyByName(const std::string &name) {
        auto it = properties_.find(name);
        if (it != properties_.end())
            return it->second;
        return typename Property<T>::Ptr();
    }

    typename Property<T>::Ptr propertyByID(uint16_t id) {
        auto it = properties_byid_.find(id);
        if (it != properties_byid_.end())
            return it->second;
        return typename Property<T>::Ptr();
    }

    template<typename PropType>
    PropType get(const T &obj, const std::string &propname) {
        auto prop = propertyByName(propname);
        if (prop)
            return boost::any_cast<PropType>(prop->get(obj));
        return PropType();
    }

    template<typename PropType>
    void set(T &obj, const std::string &propname, const PropType &value) {
        auto prop = propertyByName(propname);
        if (prop) {
            boost::any v = value;
            prop->set(obj, v);
        }
    }

    const std::string &name() { return name_; }

    uint16_t version() { return version_; }

protected:
    std::string name_;
    PropertyContainer properties_ordered_;
    PropertyMap properties_;
    PropertyMapByID properties_byid_;

    uint16_t version_ = 0;
};

struct StructFactory {
    static StructFactory &instance() {
        static StructFactory instance_;
        return instance_;
    }

    template<typename T>
    Struct<T> &declare(const std::string name = "") {
        std::string type_name = typeid(T).name();
        std::string struct_name = name;
        if (name.empty())
            struct_name = type_name;

        auto desc = std::make_shared<Struct<T>>(struct_name);
        structs_by_typeid_[type_name] = desc;
        structs_by_name_[struct_name] = desc;
        return *desc;
    }

    template<typename T>
    Struct<T> *structByType() {
        std::string type_name = typeid(T).name();
        auto it = structs_by_typeid_.find(type_name);
        if (it != structs_by_typeid_.end())
            return static_cast<Struct<T> *>(it->second.get());
        return NULL;
    }

    template<typename T>
    Struct<T> *structByName(const std::string &name) {
        auto it = structs_by_name_.find(name);
        if (it != structs_by_name_.end())
            return static_cast<Struct<T> *>(it->second.get());
        return NULL;
    }

protected:
    typedef std::unordered_map<std::string, std::shared_ptr<StructBase>> Structs;

    Structs structs_by_typeid_;
    Structs structs_by_name_;
};

#endif //TINYWORLD_TINYREFLECTION_H
