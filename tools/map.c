#include <string.h>
#include <stdlib.h>
#include "map.h"

#define TABLE_SIZE 4096
struct node { hash_t h; struct node *next; char val[]; };
static struct node *map[TABLE_SIZE];

hash_t fnv_hash(char *s, int n, hash_t seed)
{
  int i;
  hash_t h = seed;
  for (i = 0; i < n; ++i) h = (h * FNV_PRIME) ^ s[i];
  return h;
}

void map_insert(char *key, char *val, int size)
{
  int keylen = strlen(key);
  struct node *node = malloc(sizeof *node + size);
  node->h = fnv_hash(key, keylen, FNV_SEED); node->next = NULL;
  memcpy(node->val, val, size);

  struct node **p = &map[node->h % TABLE_SIZE];
  while (*p != NULL) p = &((*p)->next);
  *p = node;
}

char *map_lookup(char *key, hash_t seed)
{
  struct node *curr;
  int keylen = strlen(key);
  hash_t h = fnv_hash(key, keylen, seed);
  for (curr = map[h % TABLE_SIZE]; curr != NULL; curr = curr->next)
    if (curr->h == h) return curr->val;
  return NULL;
}
