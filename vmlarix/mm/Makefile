
C_OBJS = phys_mem.o

ASM_OBJS =

OBJS = $(C_OBJS) $(ASM_OBJS)

#MAKE = gmake
#MAKE = make


INCLUDES = -I. -I../util -I../include -I../arch/ARM/SA1110

all: libmm.a

libmm.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)


# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	rm -f *.o 

# make realclean cleans more aggressively

realclean:
	rm -f *.o *~ libmm.a 

# make depend will create a file ".depend" with all the dependencies

depend: 
	rm -f .depend 
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif

