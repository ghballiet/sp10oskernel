
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <sys/types.h>
#include <stdint.h>

extern int syscall(uint32_t call_num, ...);

int brk(void *end_data_segment);
ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, const void *buf, size_t count);
int open(const char *pathname, int flags, mode_t mode);
int32_t sys_undefined(int32_t syscall_num);
uint32_t c_SWI_handler(uint32_t syscall_num, uint32_t p1, uint32_t p2, uint32_t p3);
/* int open(const char *pathname, int flags); */


#endif
