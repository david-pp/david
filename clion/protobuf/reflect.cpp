#include <iostream>

#include "player.pb.h"

#include <google/protobuf/reflection.h>

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

using namespace google::protobuf;

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

void test_generated_1() {
    std::cout << "------------" << __PRETTY_FUNCTION__ << std::endl;
    PlayerProto p;
    set_player(p);
    dump_player(p);
//    std::cout << p.DebugString() << std::endl;
}

void test_generated_2() {
    std::cout << "------------" << __PRETTY_FUNCTION__ << std::endl;

    const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName("PlayerProto");
    const Message*    prototype  = MessageFactory::generated_factory()->GetPrototype(descriptor);

    Message* p = prototype->New();
    if (p)
    {
        set_player(*p);
        dump_player(*p);
    }
}

int main() {
    test_generated_1();
    test_generated_2();
}