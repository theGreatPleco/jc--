#include "../decl.h"
#include "ctype.h"
#include "data.h"
#include "jieidecl.h"
#include "jieimap.h"
#include "stdio.h"
#include "stdlib.h"
#include <ctype.h>
#include <jieidefs.h>

map *table = 0;

token *toks = 0;
token *prev = 0;
token *end = 0;

node *head = 0;

const byte *decl_int = "int";
const byte *byte_decl = "byte";

const byte asign = '=';
const byte method_decl = '_'; /* expected at index 0 of buffer */
const byte method_call = '.';

const byte brack_open = '{';
const byte brack_close = '}';

const byte para_open = '(';
const byte para_close = ')';

const byte end_of_statement = ';';
const byte seperator = ',';

byte heading[] = {brack_open, brack_close, para_open, para_close,
                  end_of_statement};
static token *create_token(node *n, token_t type) {
  token *next = (token *)malloc(sizeof(token));
  next->next = 0;
  next->prev = prev;
  next->val = n->val;
  next->type = type;
  prev = next;
  return next;
}

static void __append(token *__tok, int32 __offset) {
  if (!toks) {
    toks = __tok;
    end = __tok;
  } else if (__offset) {
    for (int i = 0; i < __offset; i++) {
      end = end->prev;
    }
    end = __tok;
    end = end->next;
  } else {
    end->next = __tok;
    end = __tok;
  }
}

node *leaf = 0;

static int32 check_scan(void) { return head ? true : false; }

static token_t is_main_or_decl(byte *buffer) {
  return bytescmp(buffer, (byte *)"_main") ? t_decl_main : t_decl_func;
}

static token_t check_is_type(byte *buffer) {
  if (!buffer)
    exit(-1);
  token_t type = t_none;
  if (bytescmp(buffer, (byte *)"int")) {
    type = t_decl_int;
  }
  if (bytescmp(buffer, (byte *)"byte")) {
    type = t_decl_byte;
  }
  return type;
}

static token_t check_is_keyword(byte *buffer) {
  if (bytescmp(buffer, (byte *)"yeet")) {
    return t_yeet;
  }
  return t_none;
}

static token_t check_is_heading(byte c) {
  if (c == 0)
    return t_none;
  if (c == brack_open)
    return t_brackopen;
  if (c == brack_close)
    return t_brackclose;
  if (c == para_open)
    return t_paraopen;
  if (c == para_close)
    return t_paraclose;
  if (c == end_of_statement)
    return t_eos;
  return t_none;
}

static int32 is_alp(byte *buffer) {
  printf("got %s\n", buffer);
  byte *c = buffer;
  for (; *c != 0;) {
    printf("c is %c\n", *c);
    if (!isalpha(*c))
      return false;
    c++;
  }
  return true;
}

void lex_first_pass(void) {
  jiei::map<const byte *, token_t> *vars =
      new jiei::map<const byte *, token_t>();
  token_t type = t_none;
  int32 stackpos = 0;
  for (leaf = head; leaf != 0;) {
    type = t_none;
    byte *buffer = leaf->val;
    int32 offset = 0;

    if (*buffer == '_')
      type = is_main_or_decl(buffer);
    if (*buffer == method_call)
      type = t_call_func;

    if (*buffer == '\"')
      type = t_bytes;
    if (*buffer == '=')
      type = t_asign;
    if (check_is_type(buffer))
      type = check_is_type(buffer);
    if (check_is_keyword(buffer))
      type = check_is_keyword(buffer);
    if (check_is_heading(*buffer))
      type = check_is_heading(*buffer);

    if (isdigit(*buffer))
      type = t_int;

    if (getmathop(*buffer))
      type = getmathop(*buffer);
    /* if (mapget(table, buffer).val) {
      type = *mapget(table, buffer).type;
    } */
    if (vars->keyexist(buffer)) {
      type = vars->get((const byte *)buffer);
    }
    if (end && end->type == t_decl_int) {
      type = t_int;
      vars->insert((const byte *)buffer, t_var_int);
    }
    printf("prev is %s and end is %s\n", end ? end->val : "prev is null",
           buffer);
    /* if (end && end->type == t_asign) {
      type = end->prev->type;
      mapappend(table, end->prev->val, buffer, type, -4);
    } */
    // if(end && check_is_type(end->val))
    __append(create_token(leaf, type), offset);
    if (type == t_yeet && isdigit(leaf->next->val[0])) {
      type = t_int;
      leaf = leaf->next;
      __append(create_token(leaf, type), offset);
    } else if (type == t_decl_int && is_alp(leaf->next->val)) {
      type = t_int;
      leaf = leaf->next;
      __append(create_token(leaf, type), offset);
    }
    leaf = leaf->next;
  }
}

extern "C" map *parse(node *list) {
  table = (map *)malloc(sizeof(map));
  table->init();
  head = list;
  lex_first_pass();
  for (token *t = toks; t != 0; t = t->next) {
    printf("%s: %s\n", t->val, token_t_bytes(t->type));
  }
  table->tokens = toks;
  free(leaf);
  return table;
}
