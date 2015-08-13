#define HASH_T(N)	N ## ULL
#define FNV_PRIME	HASH_T(1099511628211)
#define FNV_SEED	HASH_T(14695981039346656037)

typedef unsigned long long hash_t;
hash_t  fnv_hash(char *key, int keylen, hash_t seed);
void  map_insert(char *key, char *val, int size);
char *map_lookup(char *key, hash_t seed);
