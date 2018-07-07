#ifndef TINYWORLD_TINYREFLECTION_H
#define TINYWORLD_TINYREFLECTION_H

//
// C++结构体属性反射的演示代码(by David++ 2017.03)
//

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <type_traits>
#include <boost/any.hpp>


//
// 序列化的主模板类定义
//
template<typename T, bool ispod>
struct SerializeImpl {
    static std::string serialize(T &object);
};


template<typename T>
struct Serialize {
    static std::string serialize(T &object) {
        return SerializeImpl<T, std::is_trivial<T>::value>::serialize(object);
    }
};


//
// 属性操作的抽象类
//
template<typename T>
class PropertyBase {
public:
    typedef std::shared_ptr<PropertyBase> Ptr;

    PropertyBase(const std::string &name) : name_(name) {}

    const std::string &name() { return name_; }

public:
    // 属性的类型信息
    virtual const std::type_info &type() = 0;

    // 获取属性的值
    virtual boost::any get(T &) = 0;

    // 设置属性的值
    virtual void set(T &, boost::any &) = 0;

    // 序列化支持
    virtual bool from_bin(T &, const std::string &bin) = 0;
    virtual bool to_bin(T &, std::string &bin) = 0;

private:
    std::string name_;
};

//
// 具体的属性操作类
//
template<typename T, typename MemFn>
class Property : public PropertyBase<T> {
public:
    Property(const std::string &name, MemFn fn)
            : PropertyBase<T>(name), fn_(fn) {}

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
        bin = Serialize<PropType>::serialize(p);
        return true;
    }

private:
    MemFn fn_;
};

//
// 创建属性操作Handler的Factory Method
//
template<typename T, typename MemFn>
Property<T, MemFn> *makePropery(const std::string &name, MemFn fn) {
    return new Property<T, MemFn>(name, fn);
}


//
// 结构体的描述和反射
//
template<typename T>
struct Struct {
public:
    typedef std::vector<typename PropertyBase<T>::Ptr> PropertyContainer;
    typedef std::unordered_map<std::string, typename PropertyBase<T>::Ptr> PropertyMap;

    Struct(const std::string &name) : name_(name) {}

    T *clone() { return new T; }

    template<typename PropType>
    Struct<T> &property(const std::string &name, PropType T::* prop) {
        if (!hasPropery(name)) {
            typename PropertyBase<T>::Ptr ptr(makePropery<T>(name, std::mem_fn(prop)));
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

    typename PropertyBase<T>::Ptr getPropertyByName(const std::string &name) {
        auto it = properties_.find(name);
        if (it != properties_.end())
            return it->second;
        return typename PropertyBase<T>::Ptr();
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


//
// 结构体描述工厂
//
struct StructFactory {
    static StructFactory &instance() {
        static StructFactory instance_;
        return instance_;
    }

    template<typename T>
    Struct<T> &declare() {
        std::string name = typeid(T).name();
        Struct<T> *desc = new Struct<T>(name);
        structs_[name] = desc;
        return *desc;
    }

    template<typename T>
    Struct<T> *structByType() {
        std::string name = typeid(T).name();
        if (structs_.find(name) != structs_.end())
            return boost::any_cast<Struct<T> *>(structs_[name]);
        return NULL;
    }

private:
    typedef std::unordered_map<std::string, boost::any> Structs;

    Structs structs_;
};

#endif //TINYWORLD_TINYREFLECTION_H
