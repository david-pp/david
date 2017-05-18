#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <boost/filesystem.hpp>
#include <map>
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

void encrypt_header(uint8_t *in, size_t len) {
    uint8_t key1[8] = {242, 162, 55, 253, 109, 63, 221, 238};
    uint8_t key2[8] = {29, 188, 85, 99, 53, 50, 85, 17};
    uint8_t key3[8] = {109, 63, 221, 238, 29, 188, 85, 99};

    DES_key_schedule keys[3];
    DES_set_key((const_DES_cblock *) key1, &keys[0]);
    DES_set_key((const_DES_cblock *) key2, &keys[1]);
    DES_set_key((const_DES_cblock *) key3, &keys[2]);

    for (size_t i = 0; i < len; i += 8) {
        DES_encrypt3((unsigned int *) (in + i), &keys[0], &keys[1], &keys[2]);
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

    std::vector<FileInfo> fileinfos;

    uint8_t *p = buf.data();
    for (int i = 0; i < header.filenum; ++i) {
        FileInfo fileinfo;
        std::memcpy(&fileinfo, p, sizeof(FileInfoHeader));
        p += sizeof(FileInfoHeader);

        char name[1024] = "";
        strncpy(name, (char *) p, sizeof(name));
        p += strlen(name) + 1;

        fileinfo.name = name;

        fileinfo.dump();

        fileinfos.push_back(fileinfo);
    }

    for (auto &file : fileinfos) {
        boost::filesystem::path p = file.name;
//        std::cout << p.parent_path() << std::endl;
        boost::filesystem::create_directories(p.parent_path());

        std::string buf;
        buf.resize(file.size);
        fs.seekg(file.offset + sizeof(PackHeader));
        fs.read(buf.data(), buf.size());

        std::ofstream ofs(file.name, std::ios::binary);
        ofs.write(buf.data(), buf.size());
    }
}

void pack(std::ostream &os, const std::string &dir) {
    using namespace boost::filesystem;

    boost::filesystem::path p(dir);

    try {
        if (exists(p)) {
            if (is_regular_file(p))
                std::cout << p << " size is " << file_size(p) << '\n';

            else if (is_directory(p)) {
                std::cout << p << " is a directory containing:\n";


                std::map<std::string, FileInfo> files;
                for (directory_entry &x : directory_iterator(p)) {

                    FileInfo fileinfo;
                    fileinfo.name = x.path().string();
                    fileinfo.id = zt2_crc32_zlib((unsigned char *) fileinfo.name.data(), fileinfo.name.length());
                    fileinfo.size = file_size(x.path());
                    fileinfo.interspace_size = fileinfo.size;
                    fileinfo.crc = 0;
                    fileinfo.offset = 0;
                    files.insert(std::make_pair(fileinfo.name, fileinfo));
//                    fileinfo.dump();
                }

                // write: pack header
                PackHeader ph;
                ph.magic = 0x4b415058;
                ph.version = 9;
                ph.filenum = files.size();
                ph.flag = 0x2;
                ph.headersize = 0;
                // headsize
                for (auto &pair : files) {
                    ph.headersize += sizeof(FileInfoHeader);
                    ph.headersize += pair.first.length() + 1;
                }

                // 8 bytes padding
                ph.headersize = (ph.headersize + 7) & (~7);
                ph.dump();

                os.write((char *) &ph, sizeof(ph));

                // write: files
                uint32_t offset = 0;
                for (auto &pair : files) {
                    std::ifstream ifs(pair.second.name, std::ios::binary);
                    pair.second.crc = zt_crc2(ifs);
                    pair.second.offset = offset;
                    pair.second.dump();

                    std::string filedata;
                    filedata.resize(pair.second.size);
                    ifs.seekg(0);
                    ifs.read(filedata.data(), pair.second.size);

                    os.seekp(offset + sizeof(PackHeader));
                    os.write(filedata.data(), filedata.size());

                    offset += pair.second.size;
                }

                // write: header
                uint32_t p = offset + sizeof(PackHeader);
                os.seekp(p);

                std::vector<uint8_t> head_buf;
                head_buf.resize(ph.headersize);
                uint32_t size = 0;
                for (auto &pair : files) {

                    memcpy(&head_buf[size], (char *) &pair.second, sizeof(FileInfoHeader));
                    size += sizeof(FileInfoHeader);

                    strcpy((char *) &head_buf[size], pair.first.c_str());
                    size += pair.first.length();

                    head_buf[size] = 0;
                    size += 1;
                }

//                std::string hex;
//                std::for_each(head_buf.begin(), head_buf.end(), [&hex](uint8_t& v)
//                {
//                    hex.push_back(v);
//                });
//
//                hexdump(hex);

                encrypt_header(head_buf.data(), head_buf.size());
                os.write(head_buf.data(), head_buf.size());
            } else
                std::cout << p << " exists, but is not a regular file or directory\n";
        } else
            std::cout << p << " does not exist\n";
    }
    catch (const filesystem_error &ex) {
        std::cout << ex.what() << '\n';
    }
}


int main(int argc, const char *argv[]) {
//    test_des();
//    return 0;
    if (argc < 3) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t" << argv[0] << " unpack file.pak" << std::endl;
        std::cout << "\t" << argv[0] << " pack   file-dir  file-dir.pak" << std::endl;
        return EXIT_FAILURE;
    }

    std::string op = argv[1];
    if ("unpack" == op) {
        std::string filename = argv[2];
        std::ifstream istrm(filename, std::ios::binary);
        if (!istrm.is_open()) {
            std::cout << "failed to open " << filename << '\n';
        } else {
            parse(istrm);
        }
    } else if ("pack" == op) {
        std::string dirname = argv[2];
        if (argc > 3) {
            std::ofstream ofs(argv[3], std::ios::binary);
            pack(ofs, dirname);
        } else {
            pack(std::cout, dirname);
        }
    }

    return EXIT_SUCCESS;
}