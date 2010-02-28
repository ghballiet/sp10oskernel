
#include <sys/types.h>
#include <kprintf.h>
#include <stdlib.h>
#include <kmalloc.h>

#define PAGESIZE 4096

/* define some fake functions for creating and destroying slabs */
void *slab_create(int pages)
{
  return malloc(pages*PAGESIZE);
}
void slab_destroy(void *slab)
{
  free(slab);
}

void panic(char *msg)
{
  printf("Kernel panic: %s\n",msg);
  exit(1);
}

void kmalloc_test()
{
  int *a,*b,*c,**a2,i,freed;

  a = (int *)kmalloc(sizeof(int));
  *a=1000;                 
  c = (int *)kmalloc((*a)*sizeof(int));
  kprintf("Size of a and c %ld\n\r",10*sizeof(int));
  kprintf(" value of a %ld\n\r",(long int)*a);
  kprintf("%lX\n",(long unsigned int)a);
  kprintf("%lX\n",(long unsigned int)c);

  a2=(int**)c;
  for(i=0;i<*a;i++)
    {
      *a2=kmalloc(2048);
      a2+=sizeof(int);
    }
  a2=(int**)c;
  for(i=0;i<*a;i++)
    {
      kfree(*a2);
      a2+=sizeof(int);
    }

  b = kmalloc(4096);
  kprintf("%lX\n",(long unsigned int)b);
  kfree(a);
  kfree(b);
  kfree(c);

  freed=kmalloc_free_some_pages();
  kprintf("I freeded the people and %d pages\n\r",freed);
  /* do a lot more testing here */

}

int main()
{
  kprintf("Initializing kmalloc\n");
  kmalloc_init();

  kprintf("Testing kmalloc\n");
  kmalloc_test();

  kprintf("\nTest Complete\n");
}
