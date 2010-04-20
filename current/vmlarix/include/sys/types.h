#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint32_t PID_t;
typedef uint32_t phys_addr;
typedef uint32_t ssize_t;
typedef uint32_t size_t;
typedef uint16_t mode_t;
typedef uint32_t dev_t;
typedef uint32_t ino_t;
typedef uint32_t nlink_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t blksize_t;
typedef uint32_t blkcnt_t; 
typedef uint32_t time_t;

/* phys_mem_t should be set according to the architecture */
typedef uint32_t phys_mem_t;

typedef int64_t off_t;

#endif
