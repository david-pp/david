#include <iostream>

#include "player.pb.h"

#include <google/protobuf/reflection.h>

using namespace google::protobuf;

//
// 演示：写一个通用的输出各种Proto的函数
//
void print_indent(int num) {
    for (int i = 0; i < num; ++i)
        std::cout << "  ";
}

void dump(const Message &p, int indent = 0) {
    auto descriptor = p.GetDescriptor();
    auto reflection = p.GetReflection();
    if (reflection && descriptor) {
        for (int i = 0; i < descriptor->field_count(); ++i) {
            auto fd = descriptor->field(i);
            if (fd) {
                print_indent(indent);
                std::cout << fd->number() << "," << fd->name() << " = ";

                if (fd->is_repeated()) {
                    std::cout << "repeated";
                } else {
                    if (FieldDescriptor::CPPTYPE_UINT32 == fd->cpp_type()) {
                        std::cout << reflection->GetUInt32(p, fd);
                    } else if (FieldDescriptor::CPPTYPE_STRING == fd->cpp_type()) {
                        std::cout << reflection->GetString(p, fd);
                    } else if (FieldDescriptor::CPPTYPE_MESSAGE == fd->cpp_type()) {
                        const Message &msg = reflection->GetMessage(p, fd);
                        std::cout << "{" << std::endl;
                        dump(msg, indent + 1);
                        std::cout << "}";
                    } else {
                        std::cout << "unkown";
                    }
                }

                std::cout << std::endl;
            }
        }
    }
}

void test_dump() {
    std::cout << "------------" << __PRETTY_FUNCTION__ << std::endl;

    std::cout << "------player" << std::endl;
    {
        PlayerProto p;
        p.set_name("david");
        p.mutable_weapon()->set_name("Sword");
        dump(p);
    }

    std::cout << "------weapon" << std::endl;
    {
        WeaponProto w;
        w.set_name("Sword");
        dump(w);
    }
}

//        enum CppType {
//            CPPTYPE_INT32       = 1,     // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
//            CPPTYPE_INT64       = 2,     // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
//            CPPTYPE_UINT32      = 3,     // TYPE_UINT32, TYPE_FIXED32
//            CPPTYPE_UINT64      = 4,     // TYPE_UINT64, TYPE_FIXED64
//            CPPTYPE_DOUBLE      = 5,     // TYPE_DOUBLE
//            CPPTYPE_FLOAT       = 6,     // TYPE_FLOAT
//            CPPTYPE_BOOL        = 7,     // TYPE_BOOL
//            CPPTYPE_ENUM        = 8,     // TYPE_ENUM
//            CPPTYPE_STRING      = 9,     // TYPE_STRING, TYPE_BYTES
//            CPPTYPE_MESSAGE     = 10,    // TYPE_MESSAGE, TYPE_GROUP


//
// 使用反射来设置PlayerProto
//
void set_player(Message &p) {

    const Reflection *reflection = p.GetReflection();
    const Descriptor *descriptor = p.GetDescriptor();
//    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName("PlayerProto");
    if (reflection && descriptor) {

        // Scalar: 1 vs N
        {
            auto fd = descriptor->FindFieldByName("id");
            if (fd && FieldDescriptor::CPPTYPE_UINT32 == fd->cpp_type()) {
                reflection->SetUInt32(&p, fd, 1024);
            }
        }

        {
            auto fd = descriptor->FindFieldByName("name");
            if (fd && FieldDescriptor::CPPTYPE_STRING == fd->cpp_type())
                reflection->SetString(&p, fd, "david");
        }

        {
            auto fd = descriptor->FindFieldByName("quests");
            if (fd && FieldDescriptor::CPPTYPE_UINT32 == fd->cpp_type() && fd->is_repeated()) {
                for (uint32 i = 0; i < 3; ++i) {
                    // 添加
                    reflection->AddUInt32(&p, fd, i);
                    // 修改
                    reflection->SetRepeatedUInt32(&p, fd, i, i + 1);
                }
            }
        }

        // Nested: 1 vs N
        {
            auto fd = descriptor->FindFieldByName("weapon");
            if (fd && FieldDescriptor::CPPTYPE_MESSAGE == fd->cpp_type()) {
                Message *weapon = reflection->MutableMessage(&p, fd);
                if (weapon) {
                    auto wd = weapon->GetDescriptor();
                    auto wr = weapon->GetReflection();

                    wr->SetUInt32(weapon, wd->FindFieldByName("type"), 47);
                    wr->SetString(weapon, wd->FindFieldByName("name"), "Sword");
                }
            }
        }

        {
            auto fd = descriptor->FindFieldByName("weapons");
            if (fd && FieldDescriptor::CPPTYPE_MESSAGE == fd->cpp_type() && fd->is_repeated()) {
                for (int i = 0; i < 3; ++i) {
                    // 添加
                    Message *weapon = reflection->AddMessage(&p, fd);
                    if (weapon) {
                        auto wd = weapon->GetDescriptor();
                        auto wr = weapon->GetReflection();

                        wr->SetUInt32(weapon, wd->FindFieldByName("type"), i);
                        wr->SetString(weapon, wd->FindFieldByName("name"), "Shield" + std::to_string(i));
                    }

                    // 修改
                    weapon = reflection->MutableRepeatedMessage(&p, fd, i);
                    if (weapon) {
                        auto wd = weapon->GetDescriptor();
                        auto wr = weapon->GetReflection();
                        wr->SetUInt32(weapon, wd->FindFieldByName("type"), i + 1);
                    }
                }
            }
        }

        // Repeated Other Way:
        {
            auto fd = descriptor->FindFieldByName("weapons");
            if (fd && FieldDescriptor::CPPTYPE_MESSAGE == fd->cpp_type() && fd->is_repeated()) {
                MutableRepeatedFieldRef<Message> repeated = reflection->GetMutableRepeatedFieldRef<Message>(&p, fd);

                const Descriptor *wd = DescriptorPool::generated_pool()->FindMessageTypeByName("WeaponProto");
                const Message *wp = MessageFactory::generated_factory()->GetPrototype(wd);
                if (wd && wp) {
                    Message *weapon = wp->New();
                    if (weapon) {
                        auto wd = weapon->GetDescriptor();
                        auto wr = weapon->GetReflection();
                        wr->SetUInt32(weapon, wd->FindFieldByName("type"), 10);
                        wr->SetString(weapon, wd->FindFieldByName("name"), "Shield10");

                        repeated.Add(*weapon);
                    }
                }
            }
        } // Repatd Other Way:
    } // reflection && descriptor
}

//
// 使用反射来获取PlayerProto的属性
//
void dump_player(const Message &p) {
    const Reflection *reflection = p.GetReflection();
    const Descriptor *descriptor = p.GetDescriptor();
//    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName("PlayerProto");
    if (reflection && descriptor) {
        // Scalar: 1 vs N
        {
            auto fd = descriptor->FindFieldByName("id");
            if (fd && FieldDescriptor::CPPTYPE_UINT32 == fd->cpp_type()) {
                std::cout << "id = " << reflection->GetUInt32(p, fd) << std::endl;
            }
        }

        {
            auto fd = descriptor->FindFieldByName("name");
            if (fd && FieldDescriptor::CPPTYPE_STRING == fd->cpp_type())
                std::cout << "name = " << reflection->GetString(p, fd) << std::endl;
        }

        {
            auto fd = descriptor->FindFieldByName("quests");
            if (fd && FieldDescriptor::CPPTYPE_UINT32 == fd->cpp_type() && fd->is_repeated()) {
                int size = reflection->FieldSize(p, fd);
                std::cout << "quests = [";
                for (int i = 0; i < size; ++i) {
                    std::cout << reflection->GetRepeatedUInt32(p, fd, i) << ",";
                }
                std::cout << "]" << std::endl;
            }
        }

        // Nested: 1 vs N
        {
            auto fd = descriptor->FindFieldByName("weapon");
            if (fd && FieldDescriptor::CPPTYPE_MESSAGE == fd->cpp_type()) {
                const Message &weapon = reflection->GetMessage(p, fd);
                auto wd = weapon.GetDescriptor();
                auto wr = weapon.GetReflection();

                std::cout << "weapon = (";
                std::cout << wr->GetUInt32(weapon, wd->FindFieldByName("type")) << ",";
                std::cout << wr->GetString(weapon, wd->FindFieldByName("name"));
                std::cout << ")" << std::endl;
            }
        }

        {
            auto fd = descriptor->FindFieldByName("weapons");
            if (fd && FieldDescriptor::CPPTYPE_MESSAGE == fd->cpp_type() && fd->is_repeated()) {
                int size = reflection->FieldSize(p, fd);
                std::cout << "weapons = [";
                for (int i = 0; i < size; ++i) {
                    const Message &weapon = reflection->GetRepeatedMessage(p, fd, i);
                    auto wd = weapon.GetDescriptor();
                    auto wr = weapon.GetReflection();

                    std::cout << "(";
                    std::cout << wr->GetUInt32(weapon, wd->FindFieldByName("type")) << ",";
                    std::cout << wr->GetString(weapon, wd->FindFieldByName("name"));
                    std::cout << "),";
                }
                std::cout << "]" << std::endl;
            }
        }

        // Repeated Other Way:
        {
            auto fd = descriptor->FindFieldByName("weapons");
            if (fd && FieldDescriptor::CPPTYPE_MESSAGE == fd->cpp_type() && fd->is_repeated()) {
                RepeatedFieldRef<Message> repeated = reflection->GetRepeatedFieldRef<Message>(p, fd);
                std::cout << "weapons = [";
                for (auto it = repeated.begin(); it != repeated.end(); ++it) {
                    const Message &weapon = *it;
                    auto wd = weapon.GetDescriptor();
                    auto wr = weapon.GetReflection();
                    if (wd && wr) {
                        std::cout << "(";
                        std::cout << wr->GetUInt32(weapon, wd->FindFieldByName("type")) << ",";
                        std::cout << wr->GetString(weapon, wd->FindFieldByName("name"));
                        std::cout << "),";
                    }
                }
                std::cout << "]" << std::endl;
            }
        } // Repatd Other Way:
    } // reflection && descriptor
}

//
// 演示：使用生成的PlayerProto
//
void test_generated_1() {
    std::cout << "------------" << __PRETTY_FUNCTION__ << std::endl;
    PlayerProto p;
    set_player(p);
    dump_player(p);
//    std::cout << p.DebugString() << std::endl;
}

//
// 演示：使用生成的"PlayerProto"对应的原型来创建对象
//
void test_generated_2() {
    std::cout << "------------" << __PRETTY_FUNCTION__ << std::endl;

    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName("PlayerProto");
    const Message *prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);

    Message *p = prototype->New();
    if (p) {
        set_player(*p);
        dump_player(*p);
    }
}

//
// 演示：动态构造的"Player"
//
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>

DescriptorPool my_pool;
DynamicMessageFactory my_factory(&my_pool);

void create_player() {
    FileDescriptorProto file_proto;
    file_proto.set_name("player.proto");

    //
    // 创建Weapon动态Message
    //
    //    message WeaponProto  {
    //            optional uint32 type = 1;
    //            optional string name = 2;
    //    }
    DescriptorProto *weapon = file_proto.add_message_type();
    {
        weapon->set_name("Weapon");

        // type
        {
            FieldDescriptorProto *fd = weapon->add_field();
            fd->set_name("type");
            fd->set_type(FieldDescriptorProto::TYPE_UINT32);
            fd->set_number(1);
            fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
        }
        // name
        {
            FieldDescriptorProto *fd = weapon->add_field();
            fd->set_name("name");
            fd->set_type(FieldDescriptorProto::TYPE_STRING);
            fd->set_number(2);
            fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
        }
    }

    //
    // 创建Player动态Message
    //
    //    message PlayerProto {
    //            // Scalar: 1 vs N
    //            optional uint32 id = 1;
    //            optional string name = 2;
    //            repeated uint32 quests = 3;
    //
    //            // Nested: 1 vs N
    //            optional WeaponProto weapon = 4;
    //            repeated WeaponProto weapons = 5;
    //    }
    {
        DescriptorProto *player = file_proto.add_message_type();
        player->set_name("Player");

        // id
        {
            FieldDescriptorProto *fd = player->add_field();
            fd->set_name("id");
            fd->set_type(FieldDescriptorProto::TYPE_UINT32);
            fd->set_number(1);
            fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
        }

        // name
        {
            FieldDescriptorProto *fd = player->add_field();
            fd->set_name("name");
            fd->set_type(FieldDescriptorProto::TYPE_STRING);
            fd->set_number(2);
            fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
        }

        // quests
        {
            FieldDescriptorProto *fd = player->add_field();
            fd->set_name("quests");
            fd->set_type(FieldDescriptorProto::TYPE_UINT32);
            fd->set_number(3);
            fd->set_label(FieldDescriptorProto::LABEL_REPEATED);
        }

        // weapon
        {
            FieldDescriptorProto *fd = player->add_field();
            fd->set_name("weapon");
            fd->set_type(FieldDescriptorProto::TYPE_MESSAGE);
            fd->set_type_name("Weapon");
            fd->set_number(4);
            fd->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
        }

        // weapons
        {
            FieldDescriptorProto *fd = player->add_field();
            fd->set_name("weapons");
            fd->set_type(FieldDescriptorProto::TYPE_MESSAGE);
            fd->set_type_name("Weapon");
            fd->set_number(5);
            fd->set_label(FieldDescriptorProto::LABEL_REPEATED);
        }

        // nested
//        {
//            DescriptorProto *player_weapon = player->add_nested_type();
//            player_weapon->CopyFrom(*weapon);
//        }

//        std::cout << player->DebugString() << std::endl;
//        std::cout << player->SerializeAsString() << std::endl;
    }
    my_pool.BuildFile(file_proto);
//    std::cout << file_proto.DebugString() << std::endl;

    std::cout << "---- player.proto ----" << std::endl;
    const FileDescriptor* file_descriptor = my_pool.FindFileByName("player.proto");
    if (file_descriptor) {
        std::cout << file_descriptor->DebugString() << std::endl;
    }
}

void test_dynamic_1() {
    std::cout << "------------" << __PRETTY_FUNCTION__ << std::endl;

    create_player();

    const Descriptor *descriptor = my_pool.FindMessageTypeByName("Player");
    if (descriptor) {
        std::cout << "---- message:Player ----" << std::endl;
        std::cout << descriptor->DebugString() << std::endl;

        std::cout << "---- create player ----" << std::endl;
        const Message *prototype = my_factory.GetPrototype(descriptor);
        if (prototype) {
            Message *p = prototype->New();
            if (p) {
                set_player(*p);
                dump_player(*p);
            }
        }
    }
}

//
// 演示：动态编译的PlayerProto(来自player.proto)
//
#include <google/protobuf/compiler/importer.h>
using namespace google::protobuf::compiler;
void test_dynamic_2() {
    std::cout << "------------" << __PRETTY_FUNCTION__ << std::endl;

    DiskSourceTree source_tree;

    // 在../../protobuf目录中查找*.proto源文件
    source_tree.MapPath("", "../../protobuf");

    Importer importer(&source_tree, NULL);

    // 动态编译player.proto
    importer.Import("player.proto");


    // 动态消息工程
    DynamicMessageFactory factory(importer.pool());

    const Descriptor *descriptor = importer.pool()->FindMessageTypeByName("PlayerProto");
    if (descriptor) {
        std::cout << "---- message:PlayerProto ----" << std::endl;
        cout << descriptor->DebugString();

        std::cout << "---- create:PlayerProto ----" << std::endl;
        const Message* prototype = factory.GetPrototype(descriptor);
        if (prototype) {
            Message *p = prototype->New();
            if (p) {
                set_player(*p);
                dump_player(*p);
            }
        }
    }
}

int main() {
    test_dump();
    test_generated_1();
    test_generated_2();
    test_dynamic_1();
    test_dynamic_2();
}