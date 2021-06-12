#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define FNV_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

#define HASH_MAP_SZ 255

void *hash_map[HASH_MAP_SZ] = {0};

// Assumes *data is NULL terminated cstr
uint64_t
fnv1a(const char *data)
{
    uint64_t hash = FNV_OFFSET_BASIS;
    while (*data) {
        hash = (hash ^ *data) * FNV_PRIME;
        data++;
    }

    return hash;
}

int
hash_key(const char *key)
{
    return fnv1a(key) % HASH_MAP_SZ;
}

void
hash_map_set(const char *key, void *value)
{
    hash_map[hash_key(key)] = value;
}

void *
hash_map_get(const char *key)
{
    return hash_map[hash_key(key)];
}

int
main(void)
{
    hash_map_set("Name", "Dylan Lom");
    printf("%s\n", (char *)hash_map_get("Name"));
}
