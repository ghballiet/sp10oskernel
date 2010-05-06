
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <sys/types.h>
#include <stdint.h>

extern int syscall(uint32_t call_num, ...);


int brk(void *end_data_segment);
ssize_t write(int fd, const void *buf, size_t count);

int open(const char *pathname, int flags, mode_t mode);
/* int open(const char *pathname, int flags); */

void exit(int code);


#endif
