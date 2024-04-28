#ifndef DECL_H
#define DECL_H
#include "jieidecl.h"
#include "jieidefs.h"

typedef enum {
  t_none, /* null */
  t_decl_main, t_decl_func,
  t_call_func,
  t_decl_int,t_decl_byte, t_decl_bytes,
  t_int,t_byte,t_bytes,
  t_add,t_sub,
  t_var_int, t_var_byte, t_var_bytes,
  t_brackopen,t_brackclose,
  t_paraopen,t_paraclose,
  t_yeet, /* returning keyword */
  t_asign, /* asignment keyword '=' */
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

static token_t getmathop(byte c) {
  switch (c) {
  case '+':
    return t_add;
  case '-':
    return t_sub;
  default:
    return (token_t)0;
  }
}

static const byte *token_t_bytes(token_t type) {
  assert((s32)type);
  if (type == t_decl_main)
    return "main method";
  if (type == t_decl_func)
    return "declaring function";
  if (type == t_decl_int || type == t_decl_byte)
    return "declaring a type";
  if (type == t_int || type == t_byte)
    return "val";
  if (type == t_brackopen || type == t_brackclose)
    return "brack";
  if (type == t_paraopen || t_paraclose)
    return "para";
  if (type == t_eos)
    return "end of statement";
  return 0;
}

#endif
