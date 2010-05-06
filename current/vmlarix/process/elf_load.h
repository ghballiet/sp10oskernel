
#ifndef ELF_LOAD_H
#define ELF_LOAD_H
#include <process.h>

#define ELF_ERROR_NO_PROGRAM_HEADER -1
#define ELF_ERROR_FILE_NOT_FOUND -2
#define ELF_ERROR_NOT_AN_EXECUTABLE -3

/* The ELF loader opens a file, checks to see that it is ELF,
   and loads it into memory.  It returns a pointer to the
   program entry point, or NULL on failure */
void *elf_load(char *filename, proc_rec* prec);


#endif

