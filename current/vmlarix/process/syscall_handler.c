#include <sys/syscall.h>
#include <process.h>
#include <kprintf.h>
#include <vfs.h>

// ====================
// = define sys calls =
// ====================
ssize_t sys_read(int fd, const void *buf, size_t count) {
  kprintf("SYSREAD called with %d, %s and %d\n\r",fd, buf, count);
  return -1;
}

ssize_t sys_write(int fd, const void *buf, size_t count) {
  // TODO: call vfs_write for stdin (i.e. write to stdin)
  kprintf("SYSWRITE called with %d, %s",fd, buf, count);
  return -1;
}

int sys_open(const char *pathname, int flags, mode_t mode) {
  kprintf("SYSOPEN called with %s, %d and %s\n\r",pathname,flags,mode);
  return -1;
}

// ====================
// = handle sys calls =
// ====================

int32_t sys_undefined(int32_t syscall_num)
{
  kprintf("Undefined system call: %d\n\r",syscall_num);
  return syscall_num;
}

uint32_t c_SWI_handler(uint32_t syscall_num, 
			uint32_t p1, 
			uint32_t p2, 
			uint32_t p3)
{
  switch(syscall_num) {
      case SYS_open:
       return sys_open((char *)p1,p2,p3); 
       break; 
     // case SYS_close: 
     //   return sys_close(p1); 
     //   break; 
     case SYS_read: 
       return sys_read(p1,(void *)p2,p3); 
       break; 
     case SYS_write: 
       return sys_write(p1,(void *)p2,p3); 
       break; 
    default:
      return sys_undefined(syscall_num);
      break;
    }
}