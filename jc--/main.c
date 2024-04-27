#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <jieidecl.h>
#define extern_ 
#include "decl.h"
#include "libs/data.h"
#include "libs/map.h"


void printusage(void){
  printf("%s\n","usage is: jqc file.jql");
}

void init(void){
  putback = 0;
  line = 1;
  curr = 1;
}

static FILE *open_stream(byte *fname){
  FILE *STREAM = fopen(fname, "r");
  if(!STREAM){
    printf("cannot open file: %s\n", fname);
    return false; 
  }
  printf("opened file: %s\n", fname);
  return STREAM;
}

static void call_lib_functions(FILE *stream) 
{
  void *handler = 0; 
  int32 flags = RTLD_LAZY;
  
  handler = dlopen("./libs/scan.so", RTLD_LAZY);
  if(!handler)
  {
    printf("unable to open library scan.so due to error %s\n",dlerror());
    exit(-1);
  }
  node *(*scan)(FILE*);
  scan = (node*(*)(FILE*))dlsym(handler,"scan");
  if(!scan)
  {
    printf("unable to call function scan due to error %s\n", dlerror());
    exit(-1);
  }
  node *head = scan(stream);
  
  map *(*parse)(node*);
  dlclose(handler);
  handler = dlopen("./libs/parse.so", flags);
  if(!handler)
  {
    printf("error when opening shared object parse.so due to error %s\n", dlerror());
  }
  parse = (map*(*)(node*))dlsym(handler, "parse");
  if(!parse)
  {
    printf("unable to call function parse due to error %s\n", dlerror());
    exit(-1);
  }
  map *ls = parse(head);
  dlclose(handler);
  handler = dlopen("./libs/gen.so", flags);
  if(!handler)
  {
    printf("error opening shared object to error %s\n", dlerror());
    exit(-1);
  }
  void (*gen)(map *table);
  gen = (void(*)(map*))dlsym(handler, "gen");
  if(!gen)
  {
    printf("unable to call function gen due to error %s\n", dlerror());
    exit(-1);
  }
  gen(ls);  
}

int32 main(int argc, byte **argv){
  if(argc != 2){
    /* printf("if-arg!= 2\n");
    printusage(); */ 
    argv[1] = "main.c-";
  }
  init();
  FILE *stream = open_stream(argv[1]);
  call_lib_functions(stream);
  return 0;
}
