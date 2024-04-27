#include "../decl.h"
#include "data.h"
#include "jieidecl.h"
#include "jieidefs.h"
#include "stdio.h"
#include "stdlib.h"

extern stack *initstack(stack *s) {
  s = (stack *)malloc(sizeof(stack));
  s->size = 0;
  for (int i = 0; i < sizeof(s); i++) {
    s->reg[i] = t_none;
  }
  return s;
}

extern void push(stack *s, token_t val) {
  int32 i = 0;
  for (; i++ < s->size;)
    ;
  s->reg[i] = val;
  s->top = s->reg[i];
  s->size++;
}

extern int32 isempty(stack *s) { return s->top; }

extern token_t peek(stack *s) { return s->top; }

extern token_t pop(stack *s) {
  s->reg[s->size--] = t_none;
  token_t val = s->top;
  s->top = s->reg[s->size] == t_none ? s->reg[s->size] : t_none;
  return val;
}
