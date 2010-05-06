
MAKE_DIRS=

C_OBJS = SA1110.o dev_list.o mem_list.o mmap.o slabs.o process_arch.o handlers.o page_table.o pt_alloc.o 
ASM_OBJS = startup.o mmu.o cache.o interrupts.o process_asm.o 

OBJS = $(C_OBJS) $(ASM_OBJS)

INCLUDES=-I. -I../../../util -I../../include -I../../../include -I../../../process -I../../../mm

all: ../../libarch.a

../../libarch.a: $(OBJS) 
	$(AR) rcs $@ $(OBJS)


subdirs:
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) || exit 1; cd ..; \
	done

# The following is a "general" recipe for making .o from .s

.s.o:
	$(ASM) $(ASFLAGS) -o $@ $<

# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	rm -f *.o

# make realclean cleans more aggressively

realclean:
	rm -f *.o *~

# make depend will create a file ".depend" with all the dependencies

depend: 
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) depend || exit 1; cd ..; \
	done
	rm -f .depend 
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend
	for X in $(ASM_OBJS) ; do \
		echo -e "$$X: \c" >> .depend; \
		echo $$X | sed -e "s/\.o/\.s/" >> .depend; \
	done

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
