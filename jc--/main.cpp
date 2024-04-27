#include "decl.h"
#include "libs/data.h"
#include "libs/map.h"
#include <dlfcn.h>
#include <jieidecl.h>
#include <stdio.h>
#include <stdlib.h>

static void printusage(void) { printf("%s\n", "usage is: jqc file.jql"); }

FILE *stream = 0;

void init(void) {
  byte putback = 0;
  int32 line = 1;
  byte curr = 1;
}

static FILE *open_stream(byte *fname) {
  FILE *STREAM = fopen(fname, "r");
  if (!STREAM) {
    printf("cannot open file: %s\n", fname);
    return false;
  }
  printf("opened file: %s\n", fname);
  return STREAM;
}

static void call_lib_functions(FILE *stream) {
  void *handler = 0;
  int32 flags = RTLD_LAZY;

  handler = dlopen("./libs/scan.so", flags);
  if (!handler) {
    printf("unable to open library scan.so due to error %s\n", dlerror());
    exit(-1);
  }
  node *(*scan)(FILE *);
  scan = (node * (*)(FILE *)) dlsym(handler, "scan");
  if (!scan) {
    printf("unable to call function scan due to error %s\n", dlerror());
    exit(-1);
  }
  node *head = scan(stream);

  map *(*parse)(node *);
  dlclose(handler);
  handler = dlopen("./libs/parse.so", flags);
  if (!handler) {
    printf("error when opening shared object parse.so due to error %s\n",
           dlerror());
  }
  parse = (map * (*)(node *)) dlsym(handler, "parse");
  if (!parse) {
    printf("unable to call function parse due to error %s\n", dlerror());
    exit(-1);
  }
  map *ls = parse(head);
  dlclose(handler);
  handler = dlopen("./libs/gen.so", flags);
  if (!handler) {
    printf("error opening shared object to error %s\n", dlerror());
    exit(-1);
  }
  void (*gen)(map *table);
  gen = (void (*)(map *))dlsym(handler, "gen");
  if (!gen) {
    printf("unable to call function gen due to error %s\n", dlerror());
    exit(-1);
  }
  gen(ls);
}
/*TODO add more args options */
static void getargs(int32 argc, byte **argv) {
  const byte *filename = argc < 2 ? "main.c-" : argv[1];
  stream = open_stream((byte *)filename);
}

int32 main(int argc, byte **argv) {
  getargs(argc, argv);
  init();
  call_lib_functions(stream);
  return 0;
}
