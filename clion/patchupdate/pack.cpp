#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <boost/filesystem.hpp>
#include <map>
#include "zt.h"
//#include "dds.h"
#include "ddsreader.hpp"

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

struct alignas(1) FileSizeInfo {
    uint32_t version;
    uint32_t flags;
    uint32_t uncompress_size;
    uint32_t crc;

    void dump() const {
        std::cout << "FILE INFO:" << version << "," << flags << uncompress_size << std::endl;
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

    std::cout << "write----------------" << std::endl;

    for (auto &file : fileinfos) {
        boost::filesystem::path p = file.name;
//        std::cout << p.parent_path() << std::endl;
        boost::filesystem::create_directories(p.parent_path());

        std::string buf;
        buf.resize(file.size);
        fs.seekg(file.offset + sizeof(PackHeader));

        FileSizeInfo filesizeinfo;
        fs.read((uint8_t *) &filesizeinfo, sizeof(FileSizeInfo));
        fs.read(buf.data(), buf.size());

        filesizeinfo.dump();

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

// --------------------------------------------------

struct alignas(1) GroupHeader {
    uint32_t magic;    // XZGL
    uint32_t version;  // 12
    uint32_t picture_num;
    uint32_t offset;

    void dump() {
        char *m = reinterpret_cast<char *>(&magic);
        std::cout << "magic   :" << m[0] << m[1] << m[2] << m[3] << std::endl;
        std::cout << "version :" << version << std::endl;
        std::cout << "picture num :" << picture_num << std::endl;
        std::cout << "offset:" << offset << std::endl;
    }
};

struct alignas(1) GroupAniHeader {
    uint16_t version;   // 12
    uint8_t blend;
    uint8_t play_type;
    uint16_t bitmap_count;
    uint16_t frame_count;
    uint32_t ani_speed;
    uint32_t interval_time;
    uint32_t color;

    void dump() {
        std::cout << "version: " << version << " bitmaps:" << bitmap_count << " frames: " << frame_count << std::endl;
    }
};

struct alignas(1) GroupAniLocation {
    uint32_t file_offset;
    uint32_t file_size;
};

struct alignas(1) GroupBlock {
    uint8_t grp_type;  // TEXRLE=4
    union {
        uint8_t pixel_format;  // 10-暂时只处理
        uint8_t alphatype;
    };

    union {
        uint8_t alpha;
        uint16_t reserver;
    };

    uint16_t width;
    uint16_t height;

    uint16_t clip_left;
    uint16_t clip_top;
    uint16_t clip_width;
    uint16_t clip_height;

    uint32_t offset;

    void dump() {
        std::cout << "GroupBlock: " << int(grp_type) << " w=" << width << " h=" << height
                  << " cl=" << clip_left
                  << " ct=" << clip_top
                  << " cw=" << clip_width
                  << " ch=" << clip_height
                  << " offset=" << offset
                  << " pixelformat=" << int(pixel_format)
                  << std::endl;
    }
};

struct alignas(1) GroupImageBlock {
    uint32_t offset;
    uint16_t left;
    uint16_t top;
    uint16_t width;
    uint16_t height;

    void dump() const {
        std::cout << "GroupImageBlock:"
                  << " l=" << left
                  << " t=" << top
                  << " w=" << width
                  << " h=" << height
                  << " offset=" << offset
                  << std::endl;
    }
};

uint32_t get_block_count(uint32_t width) {
    uint32_t res = width % 256;
    return res == 0 ? (width / 256) : (width / 256 + 1);
}

void rle_decode_line(uint8_t *&in, size_t width, uint8_t *out) {

    const size_t mask_rle = (1 << (8 - 1));

    for (size_t i = 0; i < width;) {
        size_t flag = *in++;
        size_t len = (flag & (~mask_rle)) + 1;

        if (flag & mask_rle) {
            uint8_t value = *in++;
            memset(out, value, len);
            out += len;
            i += len;
        } else {
            memcpy(out, in, len);
            out += len;
            in += len;
            i += len;
        }
    }
}

void read_tex(GroupBlock block, const char *tex) {

    uint32_t block_num_x = get_block_count(block.clip_width);
    uint32_t block_num_y = get_block_count(block.clip_height);

    std::vector<GroupImageBlock> image_blocks;
    image_blocks.resize(block_num_x * block_num_y);

    uint8_t *data = (uint8_t *) tex;
    for (uint32_t i = 0; i < image_blocks.size(); ++i) {
        GroupImageBlock *image_block = (GroupImageBlock *) data;
        image_block->dump();

        data += sizeof(GroupImageBlock);
    }

    // ..
    if (block.pixel_format == 10) {
        uint32_t tex_blk_size = 16;
        uint32_t tex_row_count = block.clip_height / 4;
        uint32_t tex_row_size = block.clip_width / 4 * tex_blk_size;

        std::string decoded;
        decoded.resize(tex_row_count * tex_row_size);
        uint8_t *data_out = (uint8_t *) decoded.data();

        for (uint32_t i = 0; i < tex_row_count; ++i) {
            rle_decode_line(data, tex_row_size, data_out);
        }

        std::cout << "-----------" << decoded.size() << std::endl;

        DDSHEADER header;
        header.surfaceDesc.dwWidth = block.clip_width;
        header.surfaceDesc.dwHeight = block.clip_height;
        header.surfaceDesc.ddpfPixelFormat.dwRGBBitCount = 8;
        header.surfaceDesc.ddpfPixelFormat.dwFlags |= DDPF_FOURCC;
        header.surfaceDesc.ddpfPixelFormat.dwFourCC = FOURCC_DXT3;

        std::vector<uint8_t> dds;
        dds.resize(sizeof(header) + decoded.size());
        memcpy(dds.data(), &header, sizeof(header));
        memcpy(dds.data() + sizeof(header), decoded.data(), decoded.size());

        Image image = read_dds(dds);

        std::cout << "IMAGE: w=" << image.width << " height=" << image.height << " size=" << image.data.size()
                  << std::endl;

//
//        dds_header_t header;
//        memset(&header, 0, sizeof(header));
//        header.magic = 0x20534444; // 'DDS'
//        header.size = 124;
//        header.flags = DDSD_CAPS | DDSD_PIXELFORMAT;
//        header.height = block.height;
//        header.width = block.width;
//        header.pitch_or_linsize = tex_row_count;
//        header.caps.caps1 = 0;
//
//
//        header.pixelfmt.flags = DDPF_FOURCC;
//        memcpy(header.pixelfmt.fourcc, "DXT3", 4);
//
//
//        bool created = false;
//        if (!created) {
//            boost::filesystem::path p = "dds";
//            boost::filesystem::create_directories(p);
//
//            std::ofstream ofs("dds/test.dds", std::ios::binary);
//            ofs.write((char*)&header, sizeof(header));
//            ofs.write(decoded.data(), decoded.size());
//            std::cout << "created dds....." << std::endl;
//        }
    }
}

void read_xcp(std::string &xcp) {
    GroupAniHeader *head = (GroupAniHeader *) xcp.data();
    std::cout << "xcp -------------" << std::endl;
    head->dump();


    std::vector<GroupBlock> blocks;
    auto *data = xcp.data() + sizeof(GroupAniHeader);
    for (uint32_t i = 0; i < head->bitmap_count; ++i) {
        GroupBlock *block = (GroupBlock *) data;

        block->dump();
        blocks.push_back(*block);
        data += sizeof(GroupBlock);
    }

    for (auto &block : blocks) {
        read_tex(block, data + block.offset);
    }
}

void read_grp(std::ifstream &fs) {
    GroupHeader header;
    fs.read(reinterpret_cast<char *>(&header), sizeof(header));
    header.dump();

    std::vector<GroupAniLocation> ani_locations;
    ani_locations.resize(header.picture_num);

    fs.seekg(header.offset);
    fs.read((char *) ani_locations.data(), ani_locations.size() * sizeof(GroupAniLocation));

    boost::filesystem::path p = "grp";
//        std::cout << p.parent_path() << std::endl;
    boost::filesystem::create_directories(p);

    for (uint32_t i = 0; i < ani_locations.size(); ++i) {
        GroupAniLocation &ani = ani_locations[i];
        std::cout << ani.file_offset << "," << ani.file_size << std::endl;

        if (ani.file_size) {
            std::ofstream ofs("grp/" + std::to_string(i) + ".xcp", std::ios::binary);
            std::string buf;
            buf.resize(ani.file_size);

            fs.seekg(ani.file_offset + sizeof(GroupHeader));
            fs.read(buf.data(), ani.file_size);
            ofs.write(buf.data(), buf.size());

            read_xcp(buf);
        }
    }
}

struct alignas(1) GLHeaderInfoHeader {
    uint32_t indent;
    uint32_t version; // 2
    uint32_t group_count;
    uint32_t info_offset;

    void dump() {
        std::cout << "GLHeaderInfoHeader: " << version << ", groupcount=" << group_count << ", offset=" << info_offset
                  << std::endl;
    }
};

struct alignas(1) GLGroupHeaderInfo {
    uint32_t crc;
    uint32_t group_id;
    uint32_t ani_count;
    uint32_t offset;
    uint32_t ani_header_offset;

    void dump() {
        std::cout << "GroupHeaderInfo: " << group_id << ", anicount=" << ani_count << ", offset=" << offset
                  << ", ani_offset=" << ani_header_offset << std::endl;
    }
};

struct alignas(1) GLAniHeaderInfo {
    GroupAniLocation location;
    uint32_t bitmap_header_offset;

    void dump() {
        std::cout << "GLAniHeaderInfo: offset=" << location.file_offset << ", size=" << location.file_size
                  << ", bmp_header_offset=" << bitmap_header_offset << std::endl;
    }
};

void read_ghd(std::ifstream &fs) {
    GLHeaderInfoHeader header;
    fs.read(reinterpret_cast<char *>(&header), sizeof(header));
    header.dump();

    std::vector<GLGroupHeaderInfo> grp_headers;

    fs.seekg(header.info_offset);
    for (uint32_t i = 0; i < header.group_count; i++) {
        GLGroupHeaderInfo grp_header;
        fs.read(reinterpret_cast<char *>(&grp_header), sizeof(grp_header));
        grp_header.dump();
        grp_headers.push_back(grp_header);
    }

    for (auto &h : grp_headers) {
        fs.seekg(h.offset);
        GroupHeader group_header;
        fs.read(reinterpret_cast<char *>(&group_header), sizeof(group_header));
        group_header.dump();

        fs.seekg(h.ani_header_offset);
        for (uint32_t i = 0; i < h.ani_count; ++i) {
            GLAniHeaderInfo ani_header;
            fs.read(reinterpret_cast<char *>(&ani_header), sizeof(ani_header));
            ani_header.dump();
        }
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
    } else if ("grp" == op) {
        std::string filename = argv[2];
        std::ifstream istrm(filename, std::ios::binary);
        if (!istrm.is_open()) {
            std::cout << "failed to open " << filename << '\n';
        } else {
            read_grp(istrm);
        }
    } else if ("ghd" == op) {
        std::string filename = argv[2];
        std::ifstream istrm(filename, std::ios::binary);
        if (!istrm.is_open()) {
            std::cout << "failed to open " << filename << '\n';
        } else {
            read_ghd(istrm);
        }
    }

    return EXIT_SUCCESS;
}