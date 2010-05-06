
/* You will probably need to add some include files. Add -I directives
 in the Makefile to find them during the compile */
#include <elf_load.h>
#include <kprintf.h>
#include <vfs.h>
#include <fcntl.h>
#include <sys/types.h>

typedef uint32_t elf32_addr;
typedef uint32_t elf32_off;
typedef uint32_t elf32_word;
typedef int32_t elf32_sword;
typedef uint16_t elf32_half;

#define EI_NIDENT 16

typedef struct 
{
  unsigned char e_ident[EI_NIDENT];  /* identify file as object file */
  elf32_half    e_type;        /* file type */
  elf32_half    e_machine;     /* required architecture */
  elf32_word    e_version;     /* object file version */
  elf32_addr    e_entry;       /* entry point address */
  elf32_off     e_phoff;       /* program header table's file offset */
  elf32_off     e_shoff;       /* section header table's file offset */
  elf32_word    e_flags;       /* processor-specific flags */
  elf32_half    e_ehsize;      /* ELF header's size in bytes */
  elf32_half    e_phentsize;   /* size of entry in program header table */
  elf32_half    e_phnum;       /* no. of entries in program header table */
  elf32_half    e_shentsize;   /* size of entry in section header table */
  elf32_half    e_shnum;       /* no. of entries in section header table */
  elf32_half    e_shstrndx;    /* section hdr index of string table */
} elf32_ehdr;


typedef struct
{
  elf32_word p_type;  /*What kind of segment this structure describes */
  elf32_off p_offset; /*Offset from beginning of file 
			to first byte of segment */
  elf32_addr p_vaddr; /*Virtual address for the first byte of the segment */
  elf32_addr p_paddr; /*ignored*/
  elf32_word p_filesz;/*Size of file in this segment*/
  elf32_word p_memsz; /*Size of memory in this segment*/
  elf32_word p_flags; /*Flags relevant to segment*/
elf32_word p_align; /*Alignment, 0 & 1 no required, else 
		      positive power of 2, 
		      should equal p_offset modulo p_align*/
} elf32_phdr;


/*Returns a pointer to the process after it's been loaded to memory*/
/* The ELF loader opens a file, checks to see that it is ELF,
   and loads it into memory.  It returns a pointer to the
   program entry point, or NULL on failure */
void *elf_load(char *filename, proc_rec *prec)
{
  elf32_ehdr elf;
  elf32_phdr phdr;
  int file_id = vfs_open(filename,O_RDONLY,0);

  kprintf("%s Fileid: %d\n\r", filename, file_id);

  if (file_id < 0) return (void *)ELF_ERROR_FILE_NOT_FOUND; /*File not found*/
  
  /*Load elf header*/
  
  vfs_read(file_id, &elf, sizeof(elf32_ehdr));
  
  kprintf("%x\n\r",elf.e_ident[0]);
  kprintf("%c\n\r",elf.e_ident[1]);
  kprintf("%c\n\r",elf.e_ident[2]);
  kprintf("%c\n\r",elf.e_ident[3]);
  kprintf("%x\n\r",elf.e_ident[4]);
  kprintf("%x\n\r",elf.e_ident[5]);
  kprintf("%x\n\r",elf.e_ident[6]);
  kprintf("%x\n\r",elf.e_ident[7]);
  kprintf("%u\n\r",elf.e_ident[15]);
  kprintf("etype %x\n\r", elf.e_type);
  kprintf("program header offset %x\n\r", elf.e_phoff);
  kprintf("phentsize %u\n\r",elf.e_phentsize);
  kprintf("phnum %u\n\r",elf.e_phnum);
  kprintf("shentsize %u\n\r",elf.e_shentsize);
  kprintf("shnum %u\n\r",elf.e_shnum);
  kprintf("Size of elf: %d\n\r", sizeof(elf));
    
  /*If no program header is to be found, this 
    field is zero and we return an error*/
  if(elf.e_phoff == 0)
    return (void *) ELF_ERROR_NO_PROGRAM_HEADER; 
  /*Wrong type of elf-file*/
  if(elf.e_type != 2)
      return (void *) ELF_ERROR_NOT_AN_EXECUTABLE; 
    
  /*Load Program Header*/
  kprintf("Loading program header\n\r");
  /*We need to seek to the offset-position where the 
    program header is located*/
  char c; int i = sizeof(elf);
  while(i++ < elf.e_phoff)
    { 
      kprintf("Spooling for the program header.. %u<%u\n\r", 
	      i, elf.e_phoff); vfs_read(file_id, &c, 1); 
    }

  /*Next read will now return the program header*/

  vfs_read(file_id, &phdr, sizeof(phdr));
  kprintf("p_type: %u\n\r", phdr.p_type);
  kprintf("p_offset: %x\n\r", phdr.p_offset);
  kprintf("p_vaddr: %x\n\r", phdr.p_vaddr);
  kprintf("p_paddr: %x\n\r", phdr.p_paddr);
  kprintf("p_filesz: %u\n\r", phdr.p_filesz);
  kprintf("p_memsz: %u\n\r", phdr.p_memsz);
  kprintf("p_flags: %u\n\r", phdr.p_flags);
  kprintf("p_align: %u\n\r", phdr.p_align);
  
  /*We need to seek to the offset-position where the 
    program segment is located*/
  i = elf.e_phoff + sizeof(phdr);
  while(i++ < phdr.p_offset)
    { 
      //kprintf("Spooling for the process.. %u<%u\n\r", i, phdr.p_offset); 
      vfs_read(file_id, &c, 1); 
    }
  kprintf("Spooled to %x\n\r", i);

  vfs_read(file_id, (void*)phdr.p_vaddr, phdr.p_filesz);

  /* zero the bss */
  char *bss_start = (char*)phdr.p_vaddr+phdr.p_filesz;
  for(i=phdr.p_filesz;i<=phdr.p_memsz;i++)
    *(bss_start++)=0;

  kprintf("Process loaded to %x, %u bytes\n\r", phdr.p_vaddr, phdr.p_filesz);
 
  vfs_close(file_id); /*Clean up*/
  
  return (void*)elf.e_entry; /* return program entry point */

}
