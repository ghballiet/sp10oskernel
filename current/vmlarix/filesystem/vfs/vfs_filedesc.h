
#ifndef FILE_DESC_H
#define FILE_DESC_H

#include <sys/types.h>
#include <vfs_mp.h>

#define NUM_FD 1024

/* This is the file descriptor structure.  The kernel
   maintains an array of file descriptors, and assigns
   them when files, devices, etc are opened */
typedef struct{
    int in_use;
    uint32_t type;
    mount_point *mp;    /* mount point of filesystem that this file is on */
    void *fs_private;  /* pointer to private data for the filesystem driver */
    uint32_t flags;    /* read/write mode, etc */
    uint32_t mode;     /* permission flags, etc */
    char *buffer;      /* I/O buffer for this file */
    uint32_t bufsize;  /* size of I/O buffer */
    uint32_t dirty;    /* set to 1 if buffer is dirty */
    int64_t curr_blk;  /* number of the fs block that is in the buffer */
    int64_t curr_log;  /* file block number that is in the buffer */
    uint32_t bufpos;   /* current read/write pointer (buffer position) */
    uint64_t filepos;  /* current read/write pointer (file position) */
    int major;     
    int minor;
}filedesc;

extern filedesc fdesc[NUM_FD];

/* Initialize the file descriptor table */
void vfs_init_filedesc();

/* Get pointer to the given file descriptor */
filedesc *fdptr(uint32_t fd);

/* Allocate a file descriptor.  Returns -1 on failure */
int alloc_fd();

/* Release a file descriptor. */
void free_fd(uint32_t fd);

#endif
