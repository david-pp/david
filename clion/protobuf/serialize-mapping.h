#ifndef CLION_SERIALIZE_MAPPING_H
#define CLION_SERIALIZE_MAPPING_H

#include <ostream>
#include <unordered_map>
#include <vector>
#include <iostream>

#include <google/protobuf/reflection.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>


template<typename T>
class ProtoMapping;

class ProtoMappingFactory;

//
// 三种类型（根据描述符的来源分类）:
//
//   generated - 使用.proto自动生成的
//   dynamic   - 根据映射规则自行构造
//   imported  - 使用.proto动态编译的
//
enum class ProtoMappingType : uint8_t {
    generated = 0,
    imported = 1,
    dynamic = 2,
};

class ProtoMappingBase {
public:
    using DescriptorPool = google::protobuf::DescriptorPool;
    using MessageFactory = google::protobuf::MessageFactory;
    using FileDescriptorProto = google::protobuf::FileDescriptorProto;

    //
    // 构造Message的描述符(只有dynamic类型有效)
    //
    virtual bool createProto() = 0;

    //
    // Message的proto定义
    //
    std::string protoDefine() {
        if (!descriptor_pool_) return "";
        auto *descriptor = descriptor_pool_->FindMessageTypeByName(proto_name_);
        if (descriptor)
            return descriptor->DebugString();
        return "";
    }

    //
    // 关联的描述符工厂和消息工厂
    //
    DescriptorPool *descriptorPool() { return descriptor_pool_; }

    MessageFactory *messageFactory() { return message_factory_; }

    void setDescriptorPool(DescriptorPool *pool) { descriptor_pool_ = pool; }

    void setMessageFactory(MessageFactory *factory) { message_factory_ = factory; }


    const std::string &cppName() { return cpp_name_; }

    const std::string &protoName() { return proto_name_; }

    void setType(ProtoMappingType type) { type_ = type; }

protected:
    DescriptorPool *descriptor_pool_ = nullptr;
    MessageFactory *message_factory_ = nullptr;

    ProtoMappingType type_ = ProtoMappingType::generated;

    std::string cpp_name_;
    std::string proto_name_;
};

template<typename T>
class ProtoMapping : public ProtoMappingBase {
public:
    ProtoMapping(Struct<T> *reflection,
                 const std::string &cpp_name,
                 const std::string &proto_name = "") {
        cpp_name_ = cpp_name;

        if (proto_name.empty())
            proto_name_ = cpp_name_ + "DynProto";
        else
            proto_name_ = proto_name;

        if (reflection) {
            struct_reflection = reflection;
            from_struct_factory_ = true;
        } else {
            struct_reflection = new Struct<T>(cpp_name_);
            from_struct_factory_ = false;
        }
    }

    virtual ~ProtoMapping() {
        if (!from_struct_factory_ && struct_reflection)
            delete struct_reflection;
    }

    template<template <typename> class SerializerT/*=DynSerializer*/, typename PropType>
    ProtoMapping<T> &property(const std::string &name, PropType T::* prop, uint16_t number) {
        struct_reflection->template property<SerializerT, PropType>(name, prop, number);
        return *this;
    }

    bool createProto() final {
        using namespace google::protobuf;

        if (!descriptor_pool_) return false;
        if (type_ != ProtoMappingType::dynamic) return false;

        FileDescriptorProto file_proto;
        file_proto.set_name(cpp_name_ + ".proto");

        DescriptorProto *descriptor = file_proto.add_message_type();
        descriptor->set_name(proto_name_);

        for (auto prop : struct_reflection->propertyIterator()) {
            if (typeid(uint32_t) == prop->type()) {
                FieldDescriptorProto *fd = descriptor->add_field();
                fd->set_name(prop->name());
                fd->set_type(FieldDescriptorProto::TYPE_UINT32);
                fd->set_number(prop->number());
                fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
            } else {
                FieldDescriptorProto *fd = descriptor->add_field();
                fd->set_name(prop->name());
                fd->set_type(FieldDescriptorProto::TYPE_BYTES);
                fd->set_number(prop->number());
                fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
            }
        }

        descriptor_pool_->BuildFile(file_proto);
        return true;
    }

public:
    Struct<T> *struct_reflection;
    bool from_struct_factory_;
};

class ProtoMappingFactory {
public:
    static ProtoMappingFactory &instance() {
        static ProtoMappingFactory instance_;
        return instance_;
    }

    ProtoMappingFactory() {
        descriptor_pool_ = const_cast<google::protobuf::DescriptorPool *>(google::protobuf::DescriptorPool::generated_pool());
        message_factory_ = google::protobuf::MessageFactory::generated_factory();
        type_ = ProtoMappingType::generated;
    }

    ProtoMappingType type() const { return type_; }

    //
    // 自定义映射
    //
    template<typename T>
    ProtoMapping<T> &declare(const std::string &cpp_name, const std::string &proto_name = "") {
        return createProtoMapping<T>(nullptr, cpp_name, proto_name);
    }

    //
    // 使用StructFactory定义映射
    //
    template<typename T>
    ProtoMappingFactory &mapping(const std::string &proto_name = "",
                                 StructFactory &struct_factory = StructFactory::instance()) {
        auto reflection = struct_factory.structByType<T>();
        if (reflection) {
            createProtoMapping<T>(reflection, reflection->name(), proto_name);
        } else {
            std::cerr << "[ProtoMapping] mapping failed: reflection is not exist : " << __PRETTY_FUNCTION__
                      << std::endl;
        }
        return *this;
    }

    template<typename T>
    ProtoMapping<T> *mappingByType() {
        auto it = mappings_by_typeid_.find(typeid(T).name());
        if (it != mappings_by_typeid_.end())
            return static_cast<ProtoMapping<T> *>(it->second.get());
        return NULL;
    }

    ProtoMappingBase *mappingByName(const std::string &name) {
        auto it = mappings_by_name_.find(name);
        if (it != mappings_by_name_.end())
            return it->second.get();
        return NULL;
    }


    template<typename T>
    std::string protoDefineByType() {
        ProtoMapping<T> *mapping = mappingByType<T>();
        if (mapping) {
            return mapping->protoDefine();
        }
        return "";
    }

    void generateAllProtoDefine(std::ostream &os = std::cout) {
        for (auto v : this->mappings_by_order_) {
            os << "// " << v->cppName() << " -> " << v->protoName() << std::endl;
            os << v->protoDefine();
            os << std::endl;
        }
    }

protected:
    template<typename T>
    ProtoMapping<T> &createProtoMapping(Struct<T> *reflection,
                                        const std::string &cpp_name,
                                        const std::string &proto_name) {

        auto mapping = std::make_shared<ProtoMapping<T>>(reflection, cpp_name, proto_name);

        mapping->setDescriptorPool(descriptor_pool_);
        mapping->setMessageFactory(message_factory_);
        mapping->setType(type_);

        if (mappingByType<T>())
            std::cerr << "[ProtoMapping] mappingByType exist: " << __PRETTY_FUNCTION__ << std::endl;

        if (mappingByName(cpp_name))
            std::cerr << "[ProtoMapping] mappingByName exist: " << cpp_name << " : " << __PRETTY_FUNCTION__
                      << std::endl;

        mappings_by_typeid_[typeid(T).name()] = mapping;
        mappings_by_typeid_[cpp_name] = mapping;
        mappings_by_order_.push_back(mapping);

        return *mapping;
    }

protected:
    typedef std::unordered_map<std::string, std::shared_ptr<ProtoMappingBase>> ProtoMappings;

    ProtoMappings mappings_by_typeid_;
    ProtoMappings mappings_by_name_;
    std::vector<std::shared_ptr<ProtoMappingBase>> mappings_by_order_;

    google::protobuf::DescriptorPool *descriptor_pool_;
    google::protobuf::MessageFactory *message_factory_;
    ProtoMappingType type_;
};

class DynamicProtoMappingFactory : public ProtoMappingFactory {
public:
    static DynamicProtoMappingFactory &instance() {
        static DynamicProtoMappingFactory instance_;
        return instance_;
    }

    DynamicProtoMappingFactory() {
        descriptor_pool_ = new google::protobuf::DescriptorPool();
        message_factory_ = new google::protobuf::DynamicMessageFactory(descriptor_pool_);
        type_ = ProtoMappingType::dynamic;
    }

    virtual ~DynamicProtoMappingFactory() {
        if (descriptor_pool_) delete descriptor_pool_;
        if (message_factory_) delete message_factory_;
    }

    template<typename T>
    bool createProtoByType() {
        ProtoMapping<T> *mapping = mappingByType<T>();
        if (mapping) {
            return mapping->createProto();
        }
    }

    void createAllProto() {
        for (auto v : this->mappings_by_order_) {
            v->createProto();
        }
    }
};

class ImportProtoMappingFactory : public ProtoMappingFactory {
public:
    static ImportProtoMappingFactory &instance() {
        static ImportProtoMappingFactory instance_;
        return instance_;
    }

    ImportProtoMappingFactory() {
        proto_source_tree_ = std::make_shared<google::protobuf::compiler::DiskSourceTree>();
        proto_importer_ = std::make_shared<google::protobuf::compiler::Importer>(proto_source_tree_.get(), nullptr);

        descriptor_pool_ = const_cast<google::protobuf::DescriptorPool *>(proto_importer_->pool());
        message_factory_ = new google::protobuf::DynamicMessageFactory(descriptor_pool_);
        type_ = ProtoMappingType::imported;
    }

    virtual ~ImportProtoMappingFactory() {
        if (message_factory_) delete message_factory_;
    }

    void opendDir(const std::string &path) {
        proto_source_tree_->MapPath("", path);
    }

    bool import(const std::string &proto_file) {
        return proto_importer_->Import(proto_file);
    }

private:
    std::shared_ptr<google::protobuf::compiler::DiskSourceTree> proto_source_tree_;
    std::shared_ptr<google::protobuf::compiler::Importer> proto_importer_;
};

#endif //CLION_SERIALIZE_MAPPING_H
