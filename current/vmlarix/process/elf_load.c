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
  
  // ===============
  // = DEBUG STUFF =
  // ===============
  // we want to read the first 10 bytes
  // of the binary file so we can compare them
  
  // move lseek back
  /* vfs_lseek(fd, phdr.p_offset, SEEK_SET); */
  /* unsigned char buf[10]; */
  /* vfs_read(fd, &buf, 10); */
  /* int a; */
  /* kprintf("Reading from %s:\n\r", filename); */
  /* for(a=0;a<10;a++) { */
  /*   kprintf("%d:\t%X\n\r",a,buf[a]); */
  /* } */
  // ===================
  // = END DEBUG STUFF =
  // ===================
  
  

  /* After getting the program header, load the program data

     phdr.p_offset is the offset in the file for the program data
     phdr.p_vaddr should be the same as ehdr.e_entry
     phdr.p_filesz is the number of bytes of program data in the file
     phdr.p_memsz is the number of bytes to set up in memory */
  vfs_lseek(fd, phdr.p_offset, SEEK_SET);
  if(ehdr.e_entry != phdr.p_vaddr)
    return NULL; /* consistency check */
  size = vfs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz+1);

  if(size != phdr.p_filesz+1)
    return NULL; /* consistency check */
  uint32_t i;

  /* Shift everything back one byte to account for weird leading-4
     stuff... after seeking to phdr.p_offset, the first byte read by vfs_read
     is a 4, then 3 0s, then EA, then E for taska. However, it should just be 3
     0s, then EA, then E, according to the Mac OS X system implementation of
     lseek/read. I'm not sure where the leading 4 is coming from here, and
     what's even stranger is that if I tell it to seek to phdr.p_offset+1, it
     simply replaces the leading 4 with a leading 0, and trying phdr.p_offset+2
     results in the same behavior as phdr.p_offset+2. */
  for(i=0; i<phdr.p_filesz; i++)
    *(unsigned char *)(phdr.p_vaddr+i) = *(unsigned char *)(phdr.p_vaddr+i+1);

  vfs_close(fd);

  /* finally, set up the BSS zero region */
  for(i=phdr.p_vaddr + phdr.p_filesz; i < phdr.p_vaddr + phdr.p_memsz; i++)
    *(unsigned char *)(i) = 0;
  return (void *)phdr.p_vaddr;
}
