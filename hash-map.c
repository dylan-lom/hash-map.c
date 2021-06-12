#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#define FNV_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

#define HASH_MAP_SZ 256

typedef struct {
    const char *key;
    void *value;
} HashMapElement;

HashMapElement hash_map[HASH_MAP_SZ] = {0};

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
hash_key_unsafe(const char *key)
{
    return fnv1a(key) % HASH_MAP_SZ;
}

int
hash_key(const char *key)
{
    int base_index = hash_key_unsafe(key);
    int index = base_index;

    while (hash_map[index].key != NULL && strcmp(hash_map[index].key, key) != 0) {
        index = (index + 1) % HASH_MAP_SZ;
        if (index == base_index) {
            fprintf(stderr, "ERROR: Hash map is full, unable to add key: %s\n", key);
            exit(1);
        }
    }

    return index;
}

void
hash_map_set(const char *key, void *value)
{
    int index = hash_key(key);
    hash_map[index].key = key;
    hash_map[index].value = value;
}

void *
hash_map_get(const char *key)
{
    return hash_map[hash_key(key)].value;
}

/* START DEBUG FUNCTIONS */
void
hash_map_dump(void)
{
    for (int i = 0; i < HASH_MAP_SZ; i++) {
        if (hash_map[i].key != NULL)
            // FIXME: Don't assume value is char *
            printf("(%03d) %-6s = %s\n", i, hash_map[i].key, (char *)hash_map[i].value);
    }
}


void
find_collisions(void)
{
    size_t keylen = 5;
    char *key = calloc(sizeof(key), keylen + 1);
    int index;
    for (size_t i = 0; i < keylen; i++) {
        for (size_t j = 0; j < 26; j++) {
            key[i] = 'A' + j;
            index = hash_key_unsafe(key);
            if (hash_map[index].key != NULL) {
                printf("COLLISION: %s collides with %s (%d)\n", key, hash_map[index].key, index);
            } else {
                hash_map[index].key = strdup(key);
            }
        }
    }
}

void
fill_hash_map(void)
{
    for (int i = 0; i < HASH_MAP_SZ; i++) {
        hash_map[i].key = "FILLED";
    }
}

void
test(void)
{
    find_collisions();
    fill_hash_map();
    hash_map_set("This should fail!", "This should fail!");
}
/* END DEBUG FUNCTIONS */

int
main(void)
{
    hash_map_set("Name", "Dylan Lom");
    hash_map_set("Age", "19");
    hash_map_set("Pet", "Dog");

    // COLLISION: ZZZM collides with Name (83)
    hash_map_set("ZZZM", "Name collision (avoided)");

    hash_map_dump();

    printf("==========\n");
    printf("(%03d): %s\n", hash_key("Name"), (char *)hash_map_get("Name"));
    printf("(%03d): %s\n", hash_key("Age"), (char *)hash_map_get("Age"));
}
