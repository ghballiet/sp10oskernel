
#ifndef ELF_LOAD_H
#define ELF_LOAD_H

/* The ELF loader opens a file, checks to see that it is ELF,
   and loads it into memory.  It returns a pointer to the
   program entry point, or NULL on failure */
void *elf_load(char *filename);


#endif

