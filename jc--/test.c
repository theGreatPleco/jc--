#include "jieidecl.h"
#include "stdio.h"
int main() {
  byte buff[32];
  sprintf(buff, "%d", 1);
  sprintf(buff, "%d", 2);
  printf("%s", buff);
  return 0;
}
