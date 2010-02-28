
C_OBJS = p04_main.o 
ASM_OBJS = 
EXTRA_OBJS = 

LIBDIRS = -Lutil -Ldrivers/char -Larch -Lmm -L/usr/local/gcc-arm/lib/gcc/arm-elf/4.4.3/
LIBS = -lmm -lutil -lchardrv -larch -lgcc

MAKE_DIRS = drivers util arch mm

OBJS = $(C_OBJS) $(ASM_OBJS)

#MAKE = gmake

MAKE = make

TOOLPATH =/usr/local/gcc-arm/bin/arm-elf-
LD = $(TOOLPATH)ld
CC = $(TOOLPATH)gcc
ASM = $(TOOLPATH)as
CFLAGS = -gstabs+ -c 
ASFLAGS = -gstabs+ 
DEPENDFLAGS=-M
ASM_DEPENDFLAGS=--MD

DEFINES = -D_KERNEL_ -DSIMICS

export 

INCLUDES = -Iutil -Iinclude -Idrivers/char -Imm -Iarch/include -I./arch/ARM/SA1110

vmlarix: subdirs $(OBJS) 
	$(LD) -g -T simics.ld -o vmlarix $(OBJS) $(EXTRA_OBJS) $(LIBDIRS) $(LIBS)

subdirs:
	for X1 in $(MAKE_DIRS) ; do \
		cd $$X1; $(MAKE) || exit 1; cd ..; \
	done

# The following is a "general" recipe for making .o from .s

.s.o:
	$(ASM) $(ASFLAGS) -o $@ $<

# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	for X1 in $(MAKE_DIRS) ; do \
		cd $$X1; $(MAKE) clean; cd ..; \
	done
	rm -f *.o 

# make realclean cleans more aggressively

realclean:
	for X1 in $(MAKE_DIRS) ; do \
		cd $$X1; $(MAKE) realclean; cd ..; \
	done
	rm -f vmlarix
	find . -name .depend -exec rm -rf \{\} \;
	find . -name \*.o -exec rm -rf \{\} \;
	find . -name \*~ -exec rm -rf \{\} \;
	find . -name semantic.cache\* -exec rm -rf \{\} \;
	find . -name .DS_Store -exec rm -rf \{\} \;
	find . -name \*.a -exec rm -rf \{\} \;
	find . -name \*.nfs\* -exec rm -rf \{\} \;


# make publish creates a tgz file and adds a link to the web page

publish:
	rm -f *.o *~ vmlarix
	./publish.sh

# make depend will create a file ".depend" with all the dependencies

depend: 
	for X1 in $(MAKE_DIRS) ; do \
		cd $$X1; $(MAKE) depend || exit 1; cd ..; \
	done
	rm -f .depend 
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend
	for X1 in $(ASM_OBJS) ; do \
		echo -e "$$X: \c" >> .depend; \
		echo $$X1 | sed -e "s/\.o/\.s/" >> .depend; \
	done

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
