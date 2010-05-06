#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint32_t PID_t;
typedef uint32_t ssize_t;
typedef uint32_t size_t;
typedef uint16_t mode_t;
typedef uint32_t dev_t;
typedef int64_t  off_t;
typedef uint32_t ino_t;
typedef uint32_t nlink_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t blksize_t;
typedef uint32_t blkcnt_t; 
typedef uint32_t time_t;

/* include typedefs that are architecture specific */
#include <sys/arch_types.h>

#endif
