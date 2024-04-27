#ifndef extnern_
#define extern_
#endif
#include "../decl.h"
#include "data.h"
#include "jieidefs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

const byte COMMENT_C = '#';

const byte EOS = ';';

const byte DOFR = '{';
const byte DOTO = '}';

const byte PARAOP = '(';
const byte PARACL = ')';

const byte STRINGIL = '"';

const byte CHARCANCEL = '/';

const byte SEP = ',';

byte sym[] = {EOS, DOFR, DOTO, PARAOP, PARACL, SEP, 0};

static void put_back(byte b) { putback = b; }

typedef struct {
  byte *data;
  int32 index;
  size_t size;
  size_t capacity;
  int32 readend;
} record;

record *read = 0;
static byte *mem_alloc(size_t size) {
  byte *buffer = (byte *)malloc(size);
  for (int i = 0; i < size + 1; i++) {
    buffer[i] = 0;
  }
  return buffer;
}

static byte *mem_realloc(byte *buffer) {
  size_t newsize = sizeof(buffer) * (size_t)4 * (size_t)2;
  byte *temp = (byte *)malloc(newsize);
  printf("temp size is: %ld", sizeof(temp));
  for (int i = 0; i < newsize; i++) {
    if (i > sizeof(buffer))
      temp[i] = 0;
    else
      temp[i] = buffer[i];
  }
  free(buffer);
  buffer = 0;
  buffer = temp;
  return buffer;
}

static int32 is_throwable_character(byte c) {
  return c == 0 || c == '\t' || c == EOL || c == '\r' || c == '\f' ? true
                                                                   : false;
}

static void update_record(byte element) {
  if (!read) {
    read = (record *)malloc(sizeof(record));
    read->data = mem_alloc(32);
    read->index = 0;
    read->readend = false;
    read->size = 0;
    read->capacity = 32;
  }
  if (read->readend)
    return;
  read->readend =
      element == EOF || is_throwable_character(element) ? true : false;
  if (is_throwable_character(element))
    return;
  if (read->capacity == read->index)
    read->data = mem_realloc(read->data);
  // read->capacity = sizeof(read);
  read->data[read->index++] = element;
  read->size++;
}

static byte next(void) {
  curr++;
  byte res = getc(STREAM);
  if (res == EOL) {
    line++;
    curr = 1;
  }
  if (res == '/')
    put_back('/');
  return res;
}

node *head;
node *tail;

static void init(FILE *file) {
  STREAM = file;
  if (!STREAM) {
    printf("error opening stream\n");
    exit(-1);
  }
  head = 0;
  tail = 0;
}

static node *node_create(byte *val) {
  node *n = (node *)malloc(sizeof(node));
  n->line = line;
  n->curr = curr;
  n->val = val;
  n->next = 0;
  return n;
}

static void append(byte *buffer) {
  if (*buffer == 0)
    return;
  node *n = node_create(buffer);
  if (!head) {
    head = n;
    tail = n;
  } else {
    tail->next = n;
    tail = tail->next;
  }
}

byte skip(byte end) {
  for (byte c = next(); c != end; c = next())
    ;
  byte c = next();
  printf("%c\n", c);
  return c;
}

static byte cancelchars(void) {
  put_back(CHARCANCEL);
  byte c = next();
  c = next();
  if (c == putback)
    skip(EOL);
  put_back(0);
  return c;
}

static byte *skipcollect(byte end) {
  size_t cap = 64 * sizeof(byte);
  byte *buffer = mem_alloc(cap * sizeof(*buffer));
  size_t index = 0;
  size_t size = 1;
  buffer[index++] = putback;
  for (byte c = next(); c != end;) {
    if (is_throwable_character(c))
      break;
    if (c == CHARCANCEL) {
      c = cancelchars();
    }
    if (index > cap + 2) {
      for (int32 i = 0; i < cap; i++)
        buffer = (byte *)realloc(buffer, sizeof(buffer) * 2);
      cap = sizeof(buffer);
    }
    buffer[index++] = c;
    c = next();
    size++;
  }
  append(buffer);
  return buffer;
}

static int is_sym(byte c) {
  int32 res = false;
  for (int i = 0; i < sizeof(sym); i++) {
    if (c == sym[i])
      res = true;
  }
  return res ? true : false;
}

static void split(void) {
  byte *buffer;
  int32 buf_s = 0;
  for (byte c = 0; c != EOF; c = next()) {
    buffer = mem_alloc(64);
    if (isgraph(c)) {
      int32 nappend = true;
      int32 nskip = false;
      byte cursym = 0;
      int32 skipsymscan = false;
      for (; isgraph(c); c = next()) {
        if (c == COMMENT_C) {
          skip(EOL);
          if (!*buffer)
            nappend = false;
          break;
        }
        if (c == CHARCANCEL) {
          c = cancelchars();
          continue;
        }
        if (c == STRINGIL) {
          put_back('\"');
          skipcollect('\"');
          nskip = true;
          skipsymscan = true;
          put_back(0);
          nappend = false;
        }
        if (is_sym(c)) {
          append(buffer);
          buffer = 0;
          byte *temp = mem_alloc(1);
          *temp = c;
          append(temp);
          nskip = true;
          nappend = false;
        }
        if (nskip)
          break;
        if (sizeof(buffer) == buf_s)
          buffer = mem_realloc(buffer);
        if (nappend)
          buffer[buf_s++] = c;
      }
      buf_s = 0;
      if (!nappend)
        continue;
      append(buffer);
    }
  }
  fclose(STREAM);
}

extern "C" node *scan(FILE *file) {
  init(file);
  split();
  printf("parsed it to: \n");
  for (node *n = head; n != 0; n = n) {
    printf(" %s\n", n->val);
    n = n->next;
    if (n == 0)
      break;
  }
  return head;
}
