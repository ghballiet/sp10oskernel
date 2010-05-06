
C_OBJS = main.o kmalloc.o

LIBDIRS = 
LIBS = 

OBJS = $(C_OBJS)

#MAKE = gmake
MAKE = make

TOOLPATH =
LD = $(TOOLPATH)ld
CC = $(TOOLPATH)gcc
ASM = $(TOOLPATH)as
CFLAGS = -c
DEPENDFLAGS=-M
ASM_DEPENDFLAGS=--MD

DEFINES = -D_KERNEL_ -DSIMICS

export 

INCLUDES = -I. -I ../vmlarix/include -I../vmlarix/util -I../vmlarix/arch/include 

kmalloc_test: $(OBJS) 
	$(CC) -g -o kmalloc_test $(OBJS) $(LIBDIRS) $(LIBS)

# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	rm -f kmalloc_test
	rm -f *.o 

# make realclean cleans more aggressively

realclean:
	rm -f kmalloc_test
	find . -name .depend -exec rm -rf \{\} \;
	find . -name \*.o -exec rm -rf \{\} \;
	find . -name \*~ -exec rm -rf \{\} \;
	find . -name semantic.cache\* -exec rm -rf \{\} \;
	find . -name .DS_Store -exec rm -rf \{\} \;
	find . -name \*.a -exec rm -rf \{\} \;
	find . -name \*.nfs\* -exec rm -rf \{\} \;

# make depend will create a file ".depend" with all the dependencies

depend: 
	rm -f .depend 
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
