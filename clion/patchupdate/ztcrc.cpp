#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

//////////////////////////////////////////////////

unsigned int zt2_crc32(unsigned char *message, size_t len, unsigned int crc = 0xFFFFFFFF) {
    int i, j;
    unsigned int byte;
    static unsigned int table[256];

    /* Set up the table, if necessary. */

    if (table[1] == 0) {
        for (byte = 0; byte <= 255; byte++) {
            unsigned int val = byte;

            for (j = 0; j < 8; j++)
            {
                if (val & 0x01)
                    val = 0xEDB88320 ^ (val >> 1);
                else
                    val = val >> 1;
            }

            table[byte] = val;
        }
    }

    /* Through with table setup, now calculate the CRC. */
    for (i = 0; i < len; i++) {
        byte = message[i];
        crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
    }

//    return ~crc;
    return crc;
}

unsigned int zt_crc(FILE *file) {
    char buf[4096] = "";
    size_t n = 0;
    unsigned int crc = 0xFFFFFFFF;
    while((n = fread(&buf[0], sizeof(buf[0]), sizeof(buf), file)) > 0)
        crc = zt2_crc32(buf, n, crc);
    return crc;
}

//////////////////////////////////////////////////

unsigned int zt2_crc32_zlib(unsigned char *message, size_t len, unsigned int crc = 0xFFFFFFFF) {
    return ~crc32(~crc, message, len);
}

unsigned int zt_crc2(FILE *file) {
    char buf[4096] = "";
    size_t n = 0;
    unsigned int crc = 0xFFFFFFFF;
    while((n = fread(&buf[0], sizeof(buf[0]), sizeof(buf), file)) > 0)
        crc = ~crc32(~crc, buf, n);
    return crc;
}


//////////////////////////////////////////////////

/* compress or decompress from stdin to stdout */
int main(int argc, char **argv) {
    int ret;

    /* do compression if no arguments */
    if (argc == 1) {
        printf("zt_crc32 = %x\n", zt_crc2(stdin));
        return ret;
    }
        /* otherwise, report usage */
    else {
        fputs("crcpipe usage: crcpipe < source\n", stderr);
        return 1;
    }
}
