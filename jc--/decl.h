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
  t_int_var, t_byte_var, t_bytes_var,
  t_var,
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
  default: return false;
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
  case '+': return t_add;
  case '-': return t_sub;
  default: return (token_t)0;
  }
}

static u32 isvartok(token_t type){
  switch(type){
    case t_int_var:
    case t_byte_var:
    case t_bytes_var:return true;
    default: return false;
  }
}

static token_t gettypevar(token_t type){
  switch((s32)type){
    case t_decl_int: return t_int_var;
    case t_decl_byte: return t_byte_var;
    case t_decl_bytes: return t_bytes_var;
    default: return (token_t)0;
  }
}

static u32 isref(token_t type){
  return type == t_var ? true : false;
}

static const byte *typestobytes(token_t type){
  if(type == t_int) return "32 bits";
  if(type == t_byte) return "8 bits";
  if(type == t_bytes) return "string illeteral";
  return 0;
}

static const byte *opstobytes(token_t type){
  switch((s32)type){
    case t_add: return "+";
    case t_sub: return "-";
    default: return 0;
  }
}

static const byte *keywordbytes(token_t type);
static const byte *headingbytes(token_t type);

static const byte *token_t_bytes(token_t type) {
  if (type == t_decl_func || type == t_decl_main) return "func dec";
  if (type == t_decl_int || type == t_decl_byte) return "type decl";
  if (type == t_int || type == t_byte) return typestobytes(type);
  if(gettypevar(type)) return "member var";
  if(opstobytes(type)) return opstobytes(type);
  if(type == t_asign) return "asignment";
  if(type == t_yeet) return "ret";
  if (type == t_brackopen || type == t_brackclose) return "brack";
  if (type == t_paraopen || type == t_paraclose) return "para";
  if (type == t_eos) return "end of statement";
  return 0;
}

#endif
