#include <iostream>

#include "player.pb.h"

template<typename T>
std::string serialize(const T &object) {
    std::string bin;
    return bin;
}

class Archiver {
public:
    template<typename ProtoType>
    Archiver &operator<<(const ProtoType &proto) {
        buf_.resize(buf_.size() + proto.ByteSizeLong());
        proto.SerializeToArray(&buf_[write_pos_], proto.ByteSizeLong());
        write_pos_ += proto.ByteSizeLong();
        return *this;
    }

    template<typename ProtoType>
    Archiver &operator>>(ProtoType &proto) {
        proto.ParseFromString(buf_);
    }

private:
    std::string buf_;
    size_t      write_pos_ = 0;
    size_t      read_pos_ = 0;
};

//
// Archiver ar;
// ar << obj1 << obj2 << ..;
// ar >> obj1 >> obj2 >> ..;
//


//
// 直接使用
//
// std::vector<PlayerProto>
// std::map<uint32_t, PlayerProto>
//

//
// 映射

//   message WeaponProto  {
//  optional uint32 type = 1;
//  optional string name = 2;
//  }
//
//  message PlayerProto {
//        // Scalar: 1 vs N
//        optional uint32 id = 1;
//        optional string name = 2;
//        repeated uint32 quests = 3;
//
//        // Nested: 1 vs N
//        optional WeaponProto weapon = 4;
//        repeated WeaponProto weapons = 5;
//}
//

struct Weapon {
    uint32_t type = 0;
    std::string name = "";
};

struct Player {
    uint32_t  id = 0;
    std::string name = "";
    std::set<uint32_t> quests;

    Weapon weapon;
    std::map<uint32_t, Weapon> weapons;
};

int main() {
    Archiver ar;

    // serialize
    {
        PlayerProto p;
        p.set_id(1024);
        p.set_name("david");

        ar << p;
    }

    // deserialize
    {
        PlayerProto p;
        ar >> p;
        std::cout << p.DebugString() << std::endl;
    }
    return 0;
}