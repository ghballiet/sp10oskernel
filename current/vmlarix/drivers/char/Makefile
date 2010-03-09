
C_OBJS = chardev.o 16550.o
ASM_OBJS = 

OBJS = $(C_OBJS) $(ASM_OBJS)

INCLUDES = -I. -I../../include -I../../util

all: libchardrv.a

libchardrv.a: $(OBJS)
	$(AR) rcs libchardrv.a $(OBJS)


# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	rm -f *.o 

# make realclean cleans more aggressively

realclean:
	rm -f *.o *~ *.a

# make depend will create a file ".depend" with all the dependencies

depend: 
	rm -f .depend 
	$(CC) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
