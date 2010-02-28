#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint32_t PID_t;
typedef uint32_t ssize_t;
typedef uint32_t size_t;
typedef uint16_t mode_t;
typedef uint32_t dev_t;

/* include typedefs that are architecture specific */
#include <sys/arch_types.h>

#endif
