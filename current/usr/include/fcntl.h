
#ifndef FCNTL_H
#define FCNTL_H

#define O_RDONLY        0x0000          /* open for reading only */
#define O_WRONLY        0x0001          /* open for writing only */
#define O_RDWR          0x0002          /* open for reading and writing */
#define O_ACCMODE       0x0003          /* mask for above modes */

#define O_NONBLOCK      0x0004          /* no delay, if data not ready, return and send signal later */
#define O_APPEND        0x0008          /* set append mode - file offset is set to eof before each write */
#define O_SYNC          0x0080          /* synchronous writes  */
#define O_CREAT         0x0200          /* create if nonexistant */
#define O_TRUNC         0x0400          /* truncate to zero length */
#define O_EXCL          0x0800          /* error if already exists */

/* user (file owner) has read, write and execute permission */
#define S_IRWXU 00700 
/* user has read permission */
#define S_IRUSR 00400
/*user has write permission */
#define S_IWUSR 00200 
/* user has execute permission */
#define S_IXUSR 00100
/* group has read, write and execute permission */
#define S_IRWXG 00070
/* group has read permission */
#define S_IRGRP 00040
/* group has execute permission */
#define S_IWGRP 00010
/* others have read, write and execute permission */
#define S_IRWXO 00007
/* others have read permission */
#define S_IROTH 00004 
/* others have write permission */
#define S_IWOTH 00002
/* others have execute permission */
#define S_IXOTH 00001


#endif
