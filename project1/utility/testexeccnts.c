#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define EXECCNTS_64 314
#define EXECCNTS_32 351

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: testexeccnts <pid>\n");
    exit(EXIT_FAILURE);
  }

  int ret[4];
  int res = syscall(EXECCNTS_64, atoi(argv[1]), &ret);

  printf("System call returned: %d\n", res);
  printf("%d %d %d %d\n", ret[0], ret[1], ret[2], ret[3]);
  return 0;
}
