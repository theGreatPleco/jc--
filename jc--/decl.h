#ifndef DECL_H
#define DECL_H
#include "jieidecl.h"
#include "jieidefs.h"

typedef enum {
  t_none, /* null */
  t_decl_main,
  t_decl_func,
  t_call_func,
  t_decl_int,
  t_decl_byte,
  t_int,
  t_byte,
  t_bytes,
  t_add,
  t_sub,
  t_var_int,
  t_brackopen,
  t_brackclose,
  t_paraopen,
  t_paraclose,
  t_yeet,
  t_asign,
  t_eos, /*end of statement */
} token_t;

typedef struct token {
  token *next;
  token *prev;
  byte *val;
  token_t type;
} token;

typedef struct node {
  struct node *prev;
  struct node *next;
  u32 line;
  byte curr;
  token *tok;
  byte *val;
} node;

static u32 ismathop(token_t type) {
  switch (type) {
  case t_add:
  case t_sub:
    return true;
  default:
    return false;
  }
}

static u32 isnumerictype(token_t type) {
  switch ((u32)type) {
  case t_int:
    return true;
  default:
    return 0;
  }
}

#endif
