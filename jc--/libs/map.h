#ifndef MAP_H
#define MAP_H 1
#include "../decl.h"
#include "data.h"
#include "jieidecl.h"
#include "jieidefs.h"
#include "stdlib.h"

class map {
  typedef struct chunk {
    const byte *key;
    token *tok;
    u32 isused;
    chunk *next;
  } chunk;
  const u32 MAX = (1 << 11) * sizeof(chunk);
  chunk *chunks[(1 << 11) * sizeof(chunk)];
  u32 getindex(const byte *key) {
    u32 hash = 5381;
    for (byte *trav = (byte *)key; *trav;) {
      hash = ((hash << 5) + hash) + *trav;
      assert(trav);
      trav++;
    }
    hash = hash % MAX;
    assert(hash < MAX);
    return hash;
  }

  chunk *createchunk(const byte *key, token *tok) {
    chunk *next = (chunk *)malloc(sizeof(chunk));
    next->tok = tok;
    next->key = key;
    next->isused = 1;
    next->next = 0;
    return next;
  }

public:
  map() { init(); }
  void init() {
    for (u32 i = 0; i < MAX; i++) {
      chunks[i] = createchunk(0, 0);
      chunks[i]->isused = 0;
    }
  }
  void deinit() {
    for (u32 i = 0; i < MAX; i++) {
      free(chunks[i]);
    }
    free(chunks);
  }
  void insert(const byte *key, token *tok) {
    u32 index = getindex(key);
    if (!chunks[index]->isused)
      chunks[index] = createchunk(key, tok);
    else {
      chunk *next;
      for (next = *&chunks[index]; next->isused; next = next->next)
        ;
      next->next = createchunk(key, tok);
    }
  }
  token *get(const byte *key) {
    u32 index = getindex(key);
    if (bytescmp(key, chunks[index]->key))
      return chunks[index]->tok;
    chunk *val = *&chunks[index];
    for (; val->next && val->key != key; val = val->next)
      ;
    return val->tok;
  }
};

#endif
