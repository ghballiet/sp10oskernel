
#include <stdio.h>
#include <stdlib.h>

void panic(char *message)
{
  printf("%s\n",message);
  exit(1);
}

