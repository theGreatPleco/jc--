#include "../decl.h"
#include "data.h"
#include "jieidefs.h"
#include "jieilist.h"
#include "jieimap.h"
#include "jieistack.h"
#include "map.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"

FILE *output = 0;

jiei::map<const byte *, const byte *> *usedlocs =
    new jiei::map<const byte *, const byte *>();

jiei::map<const byte *, const byte *> *hashtable =
    new jiei::map<const byte *, const byte *>();

jiei::stack<token_t> *regs = new jiei::stack<token_t>();

typedef struct {
  byte *loc;
  byte *val;
  token_t type;
} bucket;

#define Mregs                                                                  \
  { "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi", "%edp" }
#define Rregs                                                                  \
  { "%rax", "%rdi", "%rsi", "%rdx", "%r10", "%r9", "%r8" }

const byte BASEPTR[] = "%rsp";
const byte STACKPTR[] = "%rbp";

// const byte BASEPTR[] = "%esp";
// const byte STACKPTR[] = "%ebp";

const u16 LONGBYTES = 1 << 4;
const u32 INTBYTES = 1 << 3;
const u16 SHORTBYTES = 1 << 2;

const u16 BYTEBITS = 1 << 3;
const u16 LONGBITS = 1 << 6;
const u16 INTBITS = 1 << 5;
const u16 SHORTBITS = 1 << 4;

jiei::map<const byte *, const byte *> *usedpos =
    new jiei::map<const byte *, const byte *>();

int32 prec(const byte *c) {
  switch (*c) {
  case '*':
  case '/': return 2;
  case '+':
  case '-': return 1;
  default: return 0;
  }
}

int32 countargs(byte *fmt) {
  int32 amount = 0;
  for (byte *trav = fmt; *trav; trav++) {
    if (*trav == '#')
      amount++;
  }
  return amount;
}

static byte *getfmt(byte c) {
  switch (c) {
    case 's': return (byte *)"%s";
    case 'd': return (byte *)"%d";
    default: return 0;
  }
}

static void emit(const byte *fmt, ...) {
  va_list ap;
  int32 len = countargs((byte *)fmt);
  va_start(ap, fmt);
  for (byte *trav = (byte *)fmt; *trav; trav++) {
    if (*trav == '#') {
      trav++;
      byte *arg = va_arg(ap, byte *);
      fprintf(output, getfmt(*trav), arg);
    } else
      fprintf(output, "%c", *trav);
  }
  va_end(ap);
}

static void create_file(void) { output = fopen("output.s", "w"); }

static void start_text() { emit((byte *)".text\n"); }

static void ret() { emit((byte *)"ret\n"); }

static void popq(byte *reg) { emit((byte *)"popq #s\n", reg); }

int32 LFBS = 0;
int32 LFES = 0;

token *leaf = 0;

s32 getsize(token_t type) {
  switch ((s32)type) {
  case t_int:
    return INTBYTES;
  default:
    return 0;
    /*  case t_short:
        return SHORTBYTES; */
  }
}

static void emit_func(byte *fname) {
  fname = bytescmp(fname, (byte *)"_main") ? (byte *)"main" : fname;
  emit((byte *)".globl #s\n", fname);
  emit((byte *)".type #s , @function\n", fname);
  emit((byte *)"#s:\n", fname);
  jiei::stack<int> *s = new jiei::stack<int>();
  free(s);
}

static void emit_func_begin() {
  emit((byte *)"LFB#d:\n", LFBS++);
  emit((byte *)"endbr32\n");
  emit((byte *)"pushq %rbp\n");
  emit((byte *)"movq %rsp, %rbp\n");
}

static void emit_var_asign(token_t type, byte *val, int32 offset) {
  emit((byte *)"movl $#s , #d(%rbp)\n", val, offset);
}

static void emit_func_end(void) {
  popq((byte *)"%rbp");
  ret();
  emit((byte *)"LFE#d:\n", LFES++);
}
static jiei::list<bucket> *exprec(token *next) {
  jiei::list<bucket> *eq = new jiei::list<bucket>();
  jiei::stack<bucket> *ops = new jiei::stack<bucket>();
  for (; next->type != t_eos;) {
    if (isnumerictype(next->type)) {
      eq->add({0, next->val, next->type});
    }
    if (ismathop(next->type)) {
      if (ops->isempty()) {
        ops->push({0, next->val, next->type});
        if (prec(ops->peek().val) == prec(next->val))
          ops->push({0, next->val, next->type});
      } else if (prec(ops->peek().val) < prec(next->val)) {
        ops->push({0, next->val, next->type});
      } else if (prec(ops->peek().val) > prec(next->val)) {
        for (; !ops->isempty() && prec(ops->peek().val) > prec(next->val);) {
          eq->add(ops->pop());
        }
        ops->push({0, next->val, next->type});
      }
    }
    next = next->next;
  }
  for (; !ops->isempty();) {
    eq->add(ops->pop());
  }
  for (u32 i = 0; i < eq->getsize(); i++) {
    printf("equation:%s\n", eq->get(i).val);
  }
  delete ops;
  return eq;
}

jiei::stack<s32> *used = new jiei::stack<s32>();

static byte *asignval(bucket *val) {
  token_t type = val->type;
  u32 size = getsize(type);
  u32 pos = size;
  byte buff[size];
  if (used->isempty()) {
    used->push(0);
  }
  used->push(used->peek() - size);
  sprintf(buff, "%d", used->peek());
  emit("movl $#s, #d(#s)\n", val->val, used->peek(), STACKPTR);
  return val->loc;
}

static void eval(jiei::list<bucket> *s) {
  const byte *ops = "*/-+";
  jiei::stack<bucket> *eq = new jiei::stack<bucket>();
  assert(ret);
  for (u32 i = 0; i < s->getsize(); i++) {
    bucket cur = s->get(i);
    printf("cur is %s\n", cur.val);
    if (isnumerictype(cur.type)) {
      eq->push(cur);
    }
    if (ismathop(cur.type)) {
      byte buff[sizeof(u32)];
      u32 res = 0;
      assert(!eq->isempty());
      bucket r = eq->pop();
      printf("%s\n", r.val);
      bucket l = eq->pop();
      printf("%s\n", l.val);
      emit("push #s#s\n", r.val[0] == '%' ? "" : "$", r.val);
      emit("push #s#s\n", l.val[0] == '%' ? "" : "$", l.val);
      switch (*cur.val) {
      case '+':
        emit("pop %rdx\n");
        emit("pop %rbx\n");
        emit("add %rbx, %rdx\n");
        emit("mov %rdx, %rax\n");
        eq->push({0, (byte *)"%rax", t_int});
        break;
      }
    }
  }
  delete s;
  delete eq;
}

static void emitrexpr(token *tok) { eval(exprec(tok)); }

static void emit_yeet(token *tok) {
  tok = tok->next;
  assert(tok);
  emitrexpr(tok);
}

extern "C" void gen(map *__table) {
  create_file();
  start_text();
  int32 offset = 0;
  for (leaf = __table->tokens; leaf; leaf = leaf->next) {
    printf("%s\n", token_t_bytes(leaf->type));
    if (leaf->type == t_decl_func || leaf->type == t_decl_main) {
      emit_func(leaf->val);
      regs->push(leaf->type);
    }
    if (leaf->type == t_brackopen) {
      switch (regs->pop()) {
      case t_decl_func:
      case t_decl_main:
        emit_func_begin();
        regs->push(leaf->type);
        break;
      default:
        break;
      }
    }
    if (leaf->type == t_brackclose) {
      emit_func_end();
    }
    if (leaf->type == t_yeet) {
      emit_yeet(leaf);
    }
  }
  delete regs;
  fclose(output);
  /* int32 res = system("as output.s -o output.o");
  if (res)
    printf("%s\n", "error when compiling to object file");
    */
  u32 res = system("gcc output.s -no-pie -g");
  printf("%s\n",
         !res ? "was able to compile"
              : "error when linking object files"); /*error is non zero val*/
  return;
}
