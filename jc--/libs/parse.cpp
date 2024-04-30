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

static s32 isdecltype(const byte *buff){
  return bytescmp(buff, "int") || 
  bytescmp(buff, "byte") || bytescmp(buff, "bytes") ? 1 : 0;
}

node *leaf = 0;

static int32 check_scan(void) { return head ? 1 : 0; }

static token_t is_main_or_decl(byte *buffer) {
  return bytescmp(buffer, (byte *)"_main") ? t_decl_main : t_decl_func;
}

static token_t check_is_type(byte *buffer) {
  assert(buffer);
  token_t type = t_none;
  if (bytescmp(buffer, (byte *)"int"))  type = t_decl_int;
  if (bytescmp(buffer, (byte *)"byte")) type = t_decl_byte;
  return type;
}

static token_t check_is_keyword(byte *buffer) {
  if (bytescmp(buffer, (byte *)"yeet")) {
    return t_yeet;
  }
  return t_none;
}

static token_t check_is_heading(byte c) {
  if (c == 0) return t_none;
  if (c == brack_open) return t_brackopen;
  if (c == brack_close) return t_brackclose;
  if (c == para_open) return t_paraopen;
  if (c == para_close) return t_paraclose;
  if (c == end_of_statement) return t_eos;
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

jiei::map<const byte*, token_t> *vars = new jiei::map<const byte*, token_t>();

void lex_first_pass(void) {
  token_t type = (token_t)0;
  for (leaf = head; leaf;) {
    type = t_none;
    byte *buffer = leaf->val;
    int32 offset = 0;
    switch(*buffer){
      case method_decl: type = is_main_or_decl(buffer); break;
      case method_call: type = t_call_func; break;
      case '\"': type = t_bytes; break;
      case asign: type = t_asign; break;
      default: break;
    }
    if (check_is_type(buffer)) type = check_is_type(buffer);
    if (check_is_keyword(buffer)) type = check_is_keyword(buffer);
    if (check_is_heading(*buffer)) type = check_is_heading(*buffer);

    if (isdigit(*buffer)) type = t_int;
    if (getmathop(*buffer)) type = getmathop(*buffer);

    if(vars->keyexist((const byte*)buffer))  type = vars->get((const byte*)buffer);
    if (isdecltype((const byte*)buffer)) type = t_decl_int;
    if(end && gettypevar(end->type)){
      type = t_var;
      vars->insert((const byte*)buffer, gettypevar(end->type));
    }
    __append(create_token(leaf, type), offset);
    leaf = leaf->next;
  }
}

void lext_second_pass(){
  for(token *leaf = toks; leaf;){
    if(isvartok(leaf->type))table->insert(leaf->val, leaf);
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
  delete vars;
  return table;
}
