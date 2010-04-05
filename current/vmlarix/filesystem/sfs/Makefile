
C_OBJS = sfs_init.o sfs_mount.o sfs_inode.o sfs_open.o sfs_read.o sfs_write.o sfs_fileblock.o sfs_fchmod.o sfs_fcntl.o sfs_lseek.o sfs_fchown.o sfs_dup.o sfs_rename.o sfs_close.o sfs_creat.o sfs_trunc.o sfs_umount.o sfs_unlink.o sfs_mknod.o sfs_link.o sfs_fstat.o sfs_mkdir.o sfs_rmdir.o

# sfs_dir.o 

ASM_OBJS = 

OBJS = $(C_OBJS) $(ASM_OBJS)

DEFINES = -D_KERNEL_

INCLUDES = -I. -I../../include -I../../drivers/block -I../../drivers/char -I../../util -I../vfs -I../../mm


all: ../libsfs.a

../libsfs.a: $(OBJS)
	$(AR) rcs ../libsfs.a $(OBJS)


# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	rm -f *.o 

# make realclean cleans more aggressively

realclean:
	rm -f *.o *~ libfs.a

# make depend will create a file ".depend" with all the dependencies

depend: 
	rm -f .depend 
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(C_OBJS:.o=.c) > .depend

# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
