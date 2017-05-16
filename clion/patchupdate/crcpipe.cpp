#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

unsigned int my_crc32(unsigned char *message, size_t len) {
    int i, j;
    unsigned int byte, crc, mask;
    static unsigned int table[256];

    /* Set up the table, if necessary. */

    if (table[1] == 0) {
        for (byte = 0; byte <= 255; byte++) {
            crc = byte;
            for (j = 7; j >= 0; j--) {    // Do eight times.
                mask = -(crc & 1);
                crc = (crc >> 1) ^ (0xEDB88320 & mask);
            }
            table[byte] = crc;
        }
    }

    /* Through with table setup, now calculate the CRC. */
    crc = 0xFFFFFFFF;
    for (i = 0; i < len; i++) {
        byte = message[i];
        crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
    }

    return ~crc;
}

unsigned int zt2_crc32(unsigned char *message, size_t len) {
    int i, j;
    unsigned int byte, crc, mask;
    static unsigned int table[256];

    /* Set up the table, if necessary. */

    if (table[1] == 0) {
        for (byte = 0; byte <= 255; byte++) {
            crc = byte;

            for (j = 0; j < 8; j++)
            {
                if (crc & 0x01)
                    crc = 0xEDB88320 ^ (crc >> 1);
                else
                    crc = crc >> 1;
            }

            table[byte] = crc;
        }
    }

    /* Through with table setup, now calculate the CRC. */
    crc = 0xFFFFFFFF;
    for (i = 0; i < len; i++) {
        byte = message[i];
        crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
    }

//    return ~crc;
    return crc;
}

/* compress or decompress from stdin to stdout */
int main(int argc, char **argv) {
    int ret;

    /* do compression if no arguments */
    if (argc == 1) {
        char buf[4096] = ""; //
        size_t n = fread(&buf[0], sizeof(buf[0]), sizeof(buf), stdin);
//        printf("%s", buf);
        printf("filesize = %lu\n", n);
        printf("my_crc32 = %08x\n", my_crc32(buf, n));

        uLong crc = crc32(0L, Z_NULL, 0);
        printf("zl_crc   = %08x\n", crc);
        printf("zl_crc32 = %08x\n", crc32(crc, buf, n));
        printf("zt_crc32 = %x\n", zt2_crc32(buf, n));
        printf("zt_crc32 = %x\n", ~crc32(crc, buf, n));
        return ret;
    }
        /* otherwise, report usage */
    else {
        fputs("crcpipe usage: crcpipe < source\n", stderr);
        return 1;
    }
}
