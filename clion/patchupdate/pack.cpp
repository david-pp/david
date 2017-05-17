#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "zt.h"

void test_des() {
    std::string value = "hello world!";

    for (size_t i = 0; i < value.size(); i++)
    {
        std::cout << (int)value[i] << ",";
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

    for (size_t i = 0; i < value.size(); i++)
    {
        std::cout << (int)value[i] << ",";
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

void parse(std::ifstream &file) {
    PackHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));
    header.dump();

    file.seekg(0, std::ios_base::end);
    long size = file.tellg();
    std::cout << "filesize:" << size << std::endl;

    file.seekg(size - header.headersize);

    std::vector<uint8_t> buf;
    buf.resize(header.headersize);
    file.read(reinterpret_cast<char *> (buf.data()), buf.size());
}


int main(int argc, const char *argv[]) {
    test_des();
    return 0;
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