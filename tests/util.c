#include <stdint.h>
#include <stdio.h>

#include "util.h"

void hexdump(const uint8_t *buf, size_t len, size_t width) {
    for (size_t i = 0; i < len; i += width) {
        printf("%04x ", i);
        size_t j;
        for (j = i; j < len && j < i + width; ++j) {
            printf("%02x ", buf[j]);
        }
        for (; j < i + width; ++j) {
            printf("   ");
        }
        for (j = i; j < len && j < i + width; ++j) {
            int c = buf[j];
            if (c < 32 || c > 126) {
                c = '.';
            }
            printf("%c", c);
        }
        printf("\n");
    }
}

uint32_t hash_djb2(const uint8_t *data, size_t len) {
    // djb2 algorithm; see http://www.cse.yorku.ca/~oz/hash.html
    uint32_t hash = 5381;
    for (const uint8_t *top = data + len; data < top; data++) {
        hash = ((hash << 5) + hash) ^ (*data); // hash * 33 ^ data
    }
    return hash;
}

void hashdump(const uint8_t *data, size_t len, size_t block_size, size_t width) {
    const uint8_t *top = data + len;
    for (size_t i = 0; data < top;) {
        size_t n = top - data;
        if (n > block_size) {
            n = block_size;
        }
        printf("%08x ", hash_djb2(data, n));
        i += 1;
        data += n;
        if (i % width == 0 || data >= top) {
            printf("\n");
        }
    }
}
