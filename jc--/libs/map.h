#ifndef MAP_H
#define MAP_H 1
#include "../decl.h"
#include "data.h"
#include "jieidecl.h"
#include "jieidefs.h"
#include "stdlib.h"

const int32 __MAPCAP = 512;

extern_ map *mapinit() {
  map *table = (map *)malloc(sizeof(map));
  for (int i = 0; i < __MAPCAP; i++) {
    table->blocks[i].val = NULL;
    table->blocks[i].stackpos = 0;
    table->blocks[i].type = NULL;
    table->blocks[i].hashval = NULL;
    table->blocks[i].storedreg = NULL;
  }
  return table;
}

extern_ void mapappend(map *table, byte *hash, byte *val, token_t type,
                       int32 stackpos) {
  int32 index = hashbytes(hash) % __MAPCAP;
  table->blocks[index].val = val ? val : hash;
  table->blocks[index].hashval = hash;
  table->blocks[index].type = &type;
  table->blocks[index].stackpos = stackpos;
  table->blocks[index].currreg = 0;
}

extern_ block mapget(map *table, byte *hash) {
  return table->blocks[hashbytes(hash) % __MAPCAP];
}

extern_ void mapaddreg(map *table, byte *hash, byte *reg) {
  table->blocks[hashbytes(hash) % __MAPCAP].storedreg = reg;
}

extern_ void mapremove(map *table, byte *hash) {
  table->blocks[hashbytes(hash) % __MAPCAP].val = NULL;
  table->blocks[hashbytes(hash) % __MAPCAP].type = NULL;
  table->blocks[hashbytes(hash) % __MAPCAP].stackpos = 0;
}

extern_ void mapdeinit(map *table) { free(table); }
#endif
