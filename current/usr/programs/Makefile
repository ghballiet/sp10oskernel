
TASKA_OBJS = taska.o
TASKB_OBJS = taskb.o
TASKC_OBJS = taska.o
TASKD_OBJS = taskb.o

OBJS = $(TASKA_OBJS) $(TASKB_OBJS) $(TASKC_OBJS) $(TASKD_OBJS)

FORMAT = -T vmlarix_usr.ld
TASKA_FORMAT = -T vmlarix_usr_taska.ld
TASKB_FORMAT = -T vmlarix_usr_taskb.ld
#FORMAT = 

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

all: taska taskb
	cd $(RAMDISKTOOLS); make; cd $(MYDIR)
	rm -f initrd
	$(RAMDISKTOOLS)/create_ramdisk initrd 256 4096
	$(RAMDISKTOOLS)/mkfs_sfs initrd
	$(RAMDISKTOOLS)/cp_to_rd taska initrd
	$(RAMDISKTOOLS)/cp_to_rd taskb initrd
	echo "Moving new initrd to vmlarix directory!"
	mv initrd ../../vmlarix


#all: taska.bin.o taskb.bin.o taskc taskd

# this takes the ROMABLE image and makes an elf object file
#   that can be linked into the kernel image 
taska.bin.o: taska.bin
	$(LD) -r -b binary -o taska.bin.o taska.bin

# this takes the ROMABLE image and makes an elf object file
# that can be linked into the kernel image 
taskb.bin.o: taskb.bin
	$(LD) -r -b binary -o taskb.bin.o taskb.bin

# this takes the executable and makes it "ROMABLE" i.e. it is an
# exact image that can be loaded and run 
taska.bin: taska
	$(OBJCOPY) -O binary taska taska.bin

# this takes the executable and makes it "ROMABLE" i.e. it is an
# exact image that can be loaded and run 
taskb.bin: taskb
	$(OBJCOPY) -O binary taskb taskb.bin

# this builds a fairly standard executable for taska 
taska: taska.c $(TASKA_OBJS)
	$(LD) $(TASKA_FORMAT) -o taska $(TASKA_OBJS) $(LIBS)


# this builds a fairly standard executable for taskb 
taskb: taskb.c $(TASKB_OBJS)
	$(LD) $(TASKB_FORMAT) -o taskb $(TASKB_OBJS) $(LIBS)


# this builds a fairly standard executable for taskb 
taskc: $(TASKC_OBJS)
	$(LD) $(FORMAT) -o taskc $(TASKC_OBJS) $(LIBS)


# this builds a fairly standard executable for taskb 
taskd: $(TASKD_OBJS)
	$(LD) $(FORMAT) -o taskd $(TASKD_OBJS) $(LIBS)

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
