###########
#Choose one of the following:
# For Sun Solaris:
#DEFINES=-DHOST_IS_BIG_ENDIAN
# For Linux X86:
DEFINES=-DHOST_IS_LITTLE_ENDIAN
###########

CC=gcc -g 
CFLAGS=

INCLUDES=-I. -Iutil -Idrivers/block

LIBDIRS = -L.
LIBS = -lutil -lblkdrv
EXTRA_LIBS=-lvfs -lsfs

CREATE_RD_OBJS=create_ramdisk.o

MAKE_DIRS=drivers/block util

export

PROGS=create_ramdisk

default: $(PROGS)

create_ramdisk: create_ramdisk.c
	$(CC) $(DEFINES) -o create_ramdisk create_ramdisk.c

.c.o:
	$(CC) -c $(DEFINES) $(CFLAGS) $(INCLUDES) $<

BLKDRV_OBJ=blkdev.o ramdisk.o
libblkdrv.a: $(BLKDRV_OBJ)
	$(AR) rcs libblkdrv.a $(BLKDRV_OBJ)

blkdev.o:
	$(CC) -c -DPTR64 -Idrivers/block -Iutil drivers/block/blkdev.c

ramdisk.o:
	$(CC) -c -DPTR64 -Idrivers/block -Iutil drivers/block/ramdisk.c

UTIL_OBJ=bitmap.o misc.o
libutil.a: $(UTIL_OBJ)
	$(AR) rcs libutil.a $(UTIL_OBJ)

bitmap.o:
	$(CC) -c -DPTR64 -Iutil util/bitmap.c

misc.o:
	$(CC) -c -DPTR64 -Iutil util/misc.c


clean:
	rm -f *.o *.a *~ $(PROGS)
