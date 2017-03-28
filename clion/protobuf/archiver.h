#ifndef CLION_ARCHIVER_H
#define CLION_ARCHIVER_H

#include <string>
#include "archive.pb.h"

//
// 顺序必须要固定的归档器
//
class Archiver : public ArchiveProto {
public:
    template<typename T>
    Archiver &operator<<(const T &object) {
        ArchiveMemberProto *mem = this->add_members();
        if (mem) {
            mem->set_hexdata(Serializer<T>::serialize(object));
        }
        return *this;
    }

    template<typename T>
    Archiver &operator>>(T &object) {
        if (read_pos_ < this->members_size()) {
            const ArchiveMemberProto &mem = this->members(read_pos_);
            Serializer<T>::deserialize(object, mem.hexdata());
            read_pos_++;
        }
        return *this;
    }

private:
    int read_pos_ = 0;
};

#endif //CLION_ARCHIVER_H
