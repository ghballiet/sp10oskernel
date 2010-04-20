
C_OBJS = syscalls.o misc.o printf.o
ASM_OBJS = user_syscall.o crt0.o

SYSLIB_OBJS = $(C_OBJS) $(ASM_OBJS)

TOOLPATH =/usr/local/gcc-arm/bin/arm-elf-
LD = $(TOOLPATH)ld
CC = $(TOOLPATH)gcc
ASM = $(TOOLPATH)as
CFLAGS = -gstabs+ -c -DSIMICS -nostdinc
ASFLAGS = -gstabs+ 
DEPENDFLAGS=-M
ASM_DEPENDFLAGS=--MD
AR = $(TOOLPATH)ar

INCLUDES = -I. -I/usr/local/gcc-arm/include -I../include -I../../include

all: libsys.a

libsys.a: $(SYSLIB_OBJS)
	$(AR) rcs libsys.a $(SYSLIB_OBJS)

# The following is a "general" recipe for making .o from .s

.s.o:
	$(ASM) $(ASFLAGS) -o $@ $<

# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	rm -f *.o 

# make realclean cleans more aggressively

realclean:
	rm -f *.o *~ *.a

depend: 
	rm -f .depend 
	$(CC) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend
	for X in $(ASM_OBJS) ; do \
		echo -e "$$X: \c" >> .depend; \
		echo $$X | sed -e "s/\.o/\.s/" >> .depend; \
	done

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
