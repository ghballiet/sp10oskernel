#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
  
/* Initialize internal data structures for kmalloc and kfree */
void kmalloc_init();

/* Allocate size bytes of memory */
void *kmalloc(size_t size);

/* Release memory pointed to by p */
void kfree(void *p);


#endif
