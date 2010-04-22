
/* You will probably need to add some include files */
#include <elf_load.h>
#include <vfs.h>
#include <fcntl.h>

/* The ELF loader opens a file, checks to see that it is ELF,
   and loads it into memory.  It returns a pointer to the
   program entry point, or NULL on failure */
void *elf_load(char *filename)
{
  int fd = vfs_open(filename, O_RDONLY, 0);
  Elf32_Ehdr ehdr;
  int size;
  size = vfs_read(fd, &ehdr, sizeof(Elf32_Ehdr));
  if(size != sizeof(Elf32_Ehdr))
    return NULL; /* something went wrong with our read */

  /* After getting the elf header, load the program header

     ehdr.e_entry should contain address we need to copy this to (and return) 
     ehdr.e_phoff is offset in file of program header */
  vfs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  Elf32_Phdr phdr;
  size = vfs_read(fd, &phdr, sizeof(Elf32_Phdr));
  if(size != sizeof(Elf32_Phdr))
    return NULL; /* something went wrong with our read */

  /* After getting the program header, load the program data

     phdr.p_offset is the offset in the file for the program data
     phdr.p_vaddr should be the same as ehdr.e_entry
     phdr.p_filesz is the number of bytes of program data in the file
     phdr.p_memsz is the number of bytes to set up in memory */
  vfs_lseek(fd, phdr.p_offset, SEEK_SET);
  if(ehdr.e_entry != phdr.p_vaddr)
    return NULL; /* consistency check */
  size = vfs_read(fd, phdr.p_vaddr, phdr.p_filesz);

  /* finally, set up the BSS zero region */
  uint32_t i;
  for(i=0; i < phdr.p_memsz - phdr.p_filesz; i++)
    *(phdr.p_vaddr + phdr.p_filesz + i) = 0;
  return phdr.p_vaddr;
}
