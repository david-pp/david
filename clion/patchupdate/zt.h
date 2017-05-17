#ifndef CLION_ZT_H
#define CLION_ZT_H

#define DES_DEFAULT_OPTIONS
#include "des.h"
#include <string>
#include <iomanip>
#include <iostream>

inline void hexdump(const std::string &hex, std::ostream &os = std::cout) {
    size_t line = 0;
    size_t left = hex.size();

    while (left) {
        size_t linesize = (left >= 16 ? 16 : left);
        std::string linestr = hex.substr(line * 16, linesize);

        os << std::hex << std::setw(8) << std::setfill('0') << line * 16 << " ";
        for (size_t i = 0; i < linestr.size(); ++i) {
            if (i % 8 == 0)
                os << " ";

            int v = (uint8_t) linestr[i];
            os << std::hex << std::setw(2) << std::setfill('0') << v << " ";
        }

        os << " " << "|";
        for (size_t i = 0; i < linestr.size(); ++i) {
            if (std::isprint(linestr[i]))
                os << linestr[i];
            else
                os << '.';
        }
        os << "|" << std::endl;

        left -= linesize;
        line++;
    }

    os << std::dec;
}

#endif //CLION_ZT_H
