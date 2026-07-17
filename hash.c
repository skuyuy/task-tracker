#include "hash.h"

// 0x01000193 	0x811c9dc5

#define FNV1_32_PRIME        0x01000193
#define FNV1_32_OFFSET_BASIS 0x811c9dc5

size_t hash_fnv1a_32(const char *buf, size_t len) {
    if(!buf || len == 0) {
        return FNV1_32_OFFSET_BASIS;
    }

    size_t hash = FNV1_32_OFFSET_BASIS;
    for(size_t i = 0; i < len; ++i) {
        hash ^= *(buf + i);
        hash *= FNV1_32_PRIME;
    }
    return hash;
}
