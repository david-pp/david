#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "zt.h"

void test_des() {
    std::string value = "hello world!";

    for (size_t i = 0; i < value.size(); i++) {
        std::cout << (int) value[i] << ",";
    }
    std::cout << std::endl;

    uint8_t key1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t key2[8] = {2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t key3[8] = {3, 4, 5, 6, 7, 8, 9, 10};

    DES_key_schedule keys[3];
    DES_set_key((const_DES_cblock *) key1, &keys[0]);
    DES_set_key((const_DES_cblock *) key2, &keys[1]);
    DES_set_key((const_DES_cblock *) key3, &keys[2]);


    DES_encrypt3((unsigned int *) value.data(), &keys[0], &keys[1], &keys[2]);

    for (size_t i = 0; i < value.size(); i++) {
        std::cout << (int) value[i] << ",";
    }
    std::cout << std::endl;

    std::string value2 = value;

    DES_decrypt3((unsigned int *) value2.data(), &keys[0], &keys[1], &keys[2]);

    std::cout << value2 << std::endl;
}

struct alignas(1) PackHeader {
    uint32_t magic;
    uint32_t version;
    int32_t filenum;
    int32_t headersize;
    uint32_t flag;

    void dump() {
        std::cout << "magic   :" << std::hex << std::showbase << magic
                  << "(" << reinterpret_cast<char *>( &magic) << ")"
                  << std::endl;

        std::cout << std::dec;
        std::cout << "version :" << version << std::endl;
        std::cout << "filenum :" << filenum << std::endl;
        std::cout << "headersize:" << headersize << std::endl;
        std::cout << "flag    :" << std::hex << std::showbase << flag << std::endl;
        std::cout << std::dec;
    }
};

struct alignas(1) FileInfoHeader {
    uint32_t id;
    uint32_t offset;
    uint32_t size;
    uint32_t interspace_size;
    uint32_t crc;
};


struct FileInfo : public FileInfoHeader {
    std::string name;

    void dump() {
        std::cout << "---- FILE:" << name << "-------" << std::endl;
        std::cout << "id                :" << id << std::endl;
        std::cout << "offset            :" << offset << std::endl;
        std::cout << "size              :" << size << std::endl;
        std::cout << "interspace_size   :" << interspace_size << std::endl;
        std::cout << "crc               :" << std::hex << std::showbase << crc << std::endl;
        std::cout << std::dec;
    }
};

void decrypt_header(uint8_t *in, size_t len) {
    uint8_t key1[8] = {242, 162, 55, 253, 109, 63, 221, 238};
    uint8_t key2[8] = {29, 188, 85, 99, 53, 50, 85, 17};
    uint8_t key3[8] = {109, 63, 221, 238, 29, 188, 85, 99};

    DES_key_schedule keys[3];
    DES_set_key((const_DES_cblock *) key1, &keys[0]);
    DES_set_key((const_DES_cblock *) key2, &keys[1]);
    DES_set_key((const_DES_cblock *) key3, &keys[2]);

    for (size_t i = 0; i < len; i += 8) {
        DES_decrypt3((unsigned int *) (in + i), &keys[0], &keys[1], &keys[2]);
    }
}

void parse(std::ifstream &fs) {
    PackHeader header;
    fs.read(reinterpret_cast<char *>(&header), sizeof(header));
    header.dump();

    fs.seekg(0, std::ios_base::end);
    long size = fs.tellg();
    std::cout << "filesize:" << size << std::endl;

    fs.seekg(size - header.headersize);

    std::vector<uint8_t> buf;
    buf.resize(header.headersize);
    fs.read(reinterpret_cast<char *> (buf.data()), buf.size());

    decrypt_header(buf.data(), buf.size());

//    hexdump(std::string((char *) buf.data(), buf.size()));

    uint8_t *p = buf.data();
    for (int i = 0; i < header.filenum; ++i) {
        FileInfo fileinfo;
        std::memcpy(&fileinfo, p, sizeof(FileInfoHeader));
        p += sizeof(FileInfoHeader);

        char name[1024] = "";
        strncpy(name, (char*)p, sizeof(name));
        p += strlen(name) + 1;

        fileinfo.name = name;

        fileinfo.dump();
    }
}


int main(int argc, const char *argv[]) {
//    test_des();
//    return 0;
    if (argc < 2) {
        std::cerr << "Usage:" << argv[0] << " file" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    std::ifstream istrm(filename, std::ios::binary);
    if (!istrm.is_open()) {
        std::cout << "failed to open " << filename << '\n';
    } else {
        parse(istrm);
    }

    return EXIT_SUCCESS;
}