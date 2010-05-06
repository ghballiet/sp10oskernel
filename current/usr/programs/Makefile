
INIT_OBJS = init.o
LOGIN_OBJS = login.o
SHELL_OBJS = shell.o


OBJS = $(INIT_OBJS) $(LOGIN_OBJS) $(SHELL_OBJS)

FORMAT = -T vmlarix_usr.ld

TOOLPATH =/usr/local/gcc-arm/bin/arm-elf-
LD = $(TOOLPATH)ld
CC = $(TOOLPATH)gcc
ASM = $(TOOLPATH)as
OBJCOPY = $(TOOLPATH)objcopy
CFLAGS = -c
ASFLAGS = 
DEPENDFLAGS=-M
ASM_DEPENDFLAGS=--MD
AR = $(TOOLPATH)ar
DEFINES = 
RAMDISKTOOLS=../../ramdisktools
MYDIR=$(CURDIR)


INCLUDES = -I. -I/usr/local/gcc-arm/include -I../include -I../..
LIBS = -L../lib -L/usr/local/gcc-arm/lib/gcc/arm-elf/4.4.3/ -lsys -lgcc 

all: init login shell
	cd $(RAMDISKTOOLS); make; cd $(MYDIR)
	rm -f initrd
	$(RAMDISKTOOLS)/create_ramdisk initrd 256 4096
	$(RAMDISKTOOLS)/mkfs_sfs initrd
	$(RAMDISKTOOLS)/cp_to_rd init initrd
	$(RAMDISKTOOLS)/cp_to_rd login initrd
	$(RAMDISKTOOLS)/cp_to_rd shell initrd
	echo "Moving new initrd to vmlarix directory!"
	mv initrd ../../vmlarix


# this builds a fairly standard executable for init
init: init.c $(INIT_OBJS)
	$(LD) $(FORMAT) -o init $(INIT_OBJS) $(LIBS)


# this builds a fairly standard executable for login
login: login.c $(LOGIN_OBJS)
	$(LD) $(FORMAT) -o login $(LOGIN_OBJS) $(LIBS)


# this builds a fairly standard executable for the shell 
shell: shell.c $(SHELL_OBJS)
	$(LD) $(FORMAT) -o shell $(SHELL_OBJS) $(LIBS)

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
	rm -f *.o *.bin *~ taska taskb 

depend: 
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(OBJS:.o=.c) > .depend

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
