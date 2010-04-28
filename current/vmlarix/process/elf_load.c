#include <fcntl.h>
#include <stddef.h>
#include <elf_load.h>
#include <sys/types.h>
#include <vfs.h>

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
  size = vfs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
  
  // ===============
  // = DEBUG STUFF =
  // ===============
  // we want to read the first 10 bytes
  // of the binary file so we can compare them
  char buf[10];
  vfs_read(fd, &buf, sizeof(buf));
  int a;
  kprintf("Reading from %s:\n\r", filename);
  for(a=0;a<10;a++) {
    kprintf("%d:\t%X\n\r",buf[a]);
  }
  
  // move lseek back
  vfs_lseek(fd, phdr.p_offset, SEEK_SET);
  // ===================
  // = END DEBUG STUFF =
  // ===================
  
  if(size != phdr.p_filesz)
    return NULL; /* consistency check */

  /* finally, set up the BSS zero region */
  uint32_t i;
  for(i=phdr.p_vaddr + phdr.p_filesz; i < phdr.p_vaddr + phdr.p_memsz; i++)
    *(char *)(i) = 0;
  return (void *)phdr.p_vaddr;
}
