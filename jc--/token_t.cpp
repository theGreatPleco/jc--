#include "decl.h"

static const byte *headingbytes(token_t type){
  switch((u32)type){
    case t_brackopen: return "opening brack";
    case t_brackclose: return "closing brack";
    case t_paraopen: return "opeing para";
    case t_paraclose: return "closing para";
    default: return 0;
  }
}

static const byte *keywordbytes(token_t type){
  switch((u32)type){
    case t_yeet: return "ret";
    default: return 0;
  }
}
