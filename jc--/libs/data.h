#include "../decl.h"
#include "stdio.h"
#ifndef DATA_H
#define DATA_H

#ifndef extern_
#define extern_ extern
#endif

extern_ token *toks;
extern_ byte putback;
extern_ u32 line;
extern_ FILE *STREAM;

#endif
