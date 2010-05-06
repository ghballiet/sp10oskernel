
C_OBJS = process.o process_q.o elf_load.o syscall_handler.o

ASM_OBJS = 

OBJS = $(C_OBJS) $(ASM_OBJS)

INCLUDES = -I. -I../include -I../util -I../arch/include -I/usr/local/gcc-arm/lib/gcc/arm-elf/4.2.2/include -I../filesystem/vfs

all: libprocess.a

libprocess.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)


# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	rm -f *.o 

# make realclean cleans more aggressively

realclean:
	rm -f *.o *~ libprocess.a 

# make depend will create a file ".depend" with all the dependencies

depend: 
	rm -f .depend 
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif

