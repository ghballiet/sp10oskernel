###########
#Choose one of the following:
# For Sun Solaris:
#DEFINES=-DHOST_IS_BIG_ENDIAN
# For Linux X86:
DEFINES=-DHOST_IS_LITTLE_ENDIAN
###########

CC=gcc -g 
CFLAGS=
DEPENDFLAGS=-M


MAKE = make

TOOLPATH =
LD = $(TOOLPATH)ld
CC = $(TOOLPATH)gcc
ASM = $(TOOLPATH)as
CFLAGS = -c -g
ASFLAGS = -g
DEPENDFLAGS=-M
ASM_DEPENDFLAGS=--MD


INCLUDES=-I. -Iutil -Idrivers/block -Ifilesystem/sfs -Ifilesystem/vfs

LIBDIRS = -L. -Lfilesystem
LIBS = -lutil -lblkdrv
EXTRA_LIBS=-lvfs -lsfs

CREATE_RD_OBJS=create_ramdisk.o
MKFS_OBJS=mkfs_sfs.o
LSRD_OBJS=ls_rd.o
CPTORD_OBJS=cp_to_rd.o
CPFRRD_OBJS=cp_from_rd.o
RMRD_OBJS=rm_rd.o

MAKE_DIRS=drivers/block util filesystem

export

PROGS=create_ramdisk cp_from_rd cp_to_rd ls_rd mkfs_sfs

default: $(PROGS)

panic.o: panic.c
	$(CC) -c panic.c

create_ramdisk: create_ramdisk.c panic.o
	$(CC) $(DEFINES) -o create_ramdisk create_ramdisk.c panic.o

mkfs_sfs: $(MKFS_OBJS) libblkdrv.a libutil.a panic.o
	$(CC) -o mkfs_sfs $(MKFS_OBJS) $(LIBDIRS) $(LIBS) panic.o

ls_rd: $(LSRD_OBJS) libvfs.a libsfs.a libblkdrv.a libutil.a panic.o
	$(CC) -Iutil -Idrivers/block -Ifilesystem/vfs -Ifilesystem/sfs -o ls_rd $(LSRD_OBJS) $(LIBDIRS) $(EXTRA_LIBS) $(LIBS) panic.o

cp_to_rd: $(CPTORD_OBJS) libvfs.a libsfs.a libblkdrv.a libutil.a panic.o
	$(CC) -o cp_to_rd $(CPTORD_OBJS) $(LIBDIRS) $(EXTRA_LIBS) $(LIBS) panic.o

cp_from_rd: $(CPFRRD_OBJS) libvfs.a libsfs.a libblkdrv.a libutil.a panic.o
	$(CC) -o cp_from_rd $(CPFRRD_OBJS) $(LIBDIRS) $(EXTRA_LIBS) $(LIBS) panic.o

#rm_rd: $(RMRD_OBJS) libvfs.a libsfs.a libblkdrv.a libutil.a panic.o
#	$(CC) -o rm_rd $(RMRD_OBJS) $(LIBDIRS) $(EXTRA_LIBS) $(LIBS) panic.o

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

VFS_INC=-Iutil -Idrivers/block -Ifilesystem/vfs
VFS_OBJ=vfs_close.o vfs_creat.o vfs_dup.o vfs_fchmod.o vfs_fchown.o vfs_fcntl.o vfs_filedesc.o vfs_fsops.o vfs_fstat.o vfs_init.o vfs_link.o vfs_lseek.o vfs_mkdir.o vfs_mknod.o vfs_mount.o vfs_mp.o vfs_open.o vfs_pipe.o vfs_read.o vfs_rename.o vfs_rmdir.o vfs_umask.o vfs_umount.o vfs_unlink.o vfs_write.o

libvfs.a: $(VFS_OBJ)                                                         
	$(AR) rcs libvfs.a $(VFS_OBJ)                                        

vfs_close.o: filesystem/vfs/vfs_close.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_close.c

vfs_creat.o: filesystem/vfs/vfs_creat.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_creat.c

vfs_dup.o: filesystem/vfs/vfs_dup.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_dup.c

vfs_fchmod.o: filesystem/vfs/vfs_fchmod.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_fchmod.c

vfs_fchown.o: filesystem/vfs/vfs_fchown.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_fchown.c

vfs_fcntl.o: filesystem/vfs/vfs_fcntl.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_fcntl.c

vfs_filedesc.o: filesystem/vfs/vfs_filedesc.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_filedesc.c

vfs_fsops.o: filesystem/vfs/vfs_fsops.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_fsops.c

vfs_fstat.o: filesystem/vfs/vfs_fstat.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_fstat.c

vfs_init.o: filesystem/vfs/vfs_init.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_init.c

vfs_link.o: filesystem/vfs/vfs_link.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_link.c

vfs_lseek.o: filesystem/vfs/vfs_lseek.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_lseek.c

vfs_mkdir.o: filesystem/vfs/vfs_mkdir.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_mkdir.c

vfs_mknod.o: filesystem/vfs/vfs_mknod.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_mknod.c

vfs_mount.o: filesystem/vfs/vfs_mount.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_mount.c

vfs_mp.o: filesystem/vfs/vfs_mp.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_mp.c

vfs_open.o: filesystem/vfs/vfs_open.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_open.c

vfs_pipe.o: filesystem/vfs/vfs_pipe.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_pipe.c

vfs_read.o: filesystem/vfs/vfs_read.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_read.c

vfs_rename.o: filesystem/vfs/vfs_rename.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_rename.c

vfs_rmdir.o: filesystem/vfs/vfs_rmdir.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_rmdir.c

vfs_umask.o: filesystem/vfs/vfs_umask.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_umask.c

vfs_umount.o: filesystem/vfs/vfs_umount.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_umount.c

vfs_unlink.o: filesystem/vfs/vfs_unlink.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_unlink.c

vfs_write.o: filesystem/vfs/vfs_write.c
	$(CC) -c -DPTR64 $(VFS_INC) filesystem/vfs/vfs_write.c



SFS_INC=-Iutil -Idrivers/block -Ifilesystem/vfs -Ifilesystem/sfs

SFS_OBJ=sfs_fchmod.o  sfs_dup.o sfs_inode.o sfs_mknod.o sfs_rmdir.o sfs_umount.o sfs_fchown.o sfs_fcntl.o sfs_link.o sfs_mount.o sfs_read.o sfs_write.o sfs_unlink.o sfs_close.o sfs_fileblock.o sfs_lseek.o  sfs_open.o sfs_fstat.o sfs_creat.o sfs_init.o sfs_mkdir.o sfs_rename.o sfs_trunc.o

libsfs.a: $(SFS_OBJ)
	$(AR) rcs libsfs.a $(SFS_OBJ)

sfs_dir.o: filesystem/sfs/sfs_dir.c
	$(CC) -c -DPTR64 -Ifilesystem/sfs filesystem/sfs/sfs_dir.c

sfs_inode.o: filesystem/sfs/sfs_inode.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_inode.c

sfs_fileblock.o: filesystem/sfs/sfs_fileblock.c
	$(CC) -c -DPTR64 -Iutil $(SFS_INC) filesystem/sfs/sfs_fileblock.c

sfs_mount.o: filesystem/sfs/sfs_mount.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_mount.c

sfs_fileops.o: filesystem/sfs/sfs_fileops.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_fileops.c

sfs_open.o: filesystem/sfs/sfs_open.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_open.c

sfs_init.o: filesystem/sfs/sfs_init.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_init.c

sfs_read.o: filesystem/sfs/sfs_read.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_read.c

sfs_write.o: filesystem/sfs/sfs_write.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_write.c

sfs_fchmod.o: filesystem/sfs/sfs_fchmod.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_fchmod.c

sfs_dup.o: filesystem/sfs/sfs_dup.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_dup.c

sfs_mknod.o: filesystem/sfs/sfs_mknod.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_mknod.c

sfs_rmdir.o: filesystem/sfs/sfs_rmdir.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_rmdir.c

sfs_umount.o: filesystem/sfs/sfs_umount.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_umount.c

sfs_fchown.o: filesystem/sfs/sfs_fchown.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_fchown.c

sfs_fcntl.o: filesystem/sfs/sfs_fcntl.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_fcntl.c

sfs_link.o: filesystem/sfs/sfs_link.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_link.c

sfs_unlink.o: filesystem/sfs/sfs_unlink.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_unlink.c

sfs_close.o: filesystem/sfs/sfs_close.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_close.c

sfs_lseek.o: filesystem/sfs/sfs_lseek.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_lseek.c

sfs_fstat.o: filesystem/sfs/sfs_fstat.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_fstat.c

sfs_creat.o: filesystem/sfs/sfs_creat.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_creat.c

sfs_mkdir.o: filesystem/sfs/sfs_mkdir.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_mkdir.c

sfs_rename.o: filesystem/sfs/sfs_rename.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_rename.c

sfs_trunc.o: filesystem/sfs/sfs_trunc.c
	$(CC) -c -DPTR64 $(SFS_INC) filesystem/sfs/sfs_trunc.c

clean:
	rm -f *.o *.a *~ $(PROGS)

realclean:
	rm -f *.o *.a *~ $(PROGS) .depend

# make depend will create a file ".depend" with all the dependencies

depend:
	rm -f .depend
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(PROGS:=.c) > .depend
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) drivers/block/blkdev.c drivers/block/ramdisk.c >> .depend
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) util/bitmap.c util/misc.c >> .depend
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) filesystem/vfs/vfs_close.c filesystem/vfs/vfs_creat.c filesystem/vfs/vfs_dup.c filesystem/vfs/vfs_fchmod.c filesystem/vfs/vfs_fchown.c filesystem/vfs/vfs_fcntl.c filesystem/vfs/vfs_filedesc.c filesystem/vfs/vfs_fsops.c filesystem/vfs/vfs_fstat.c filesystem/vfs/vfs_init.c filesystem/vfs/vfs_link.c filesystem/vfs/vfs_lseek.c filesystem/vfs/vfs_mkdir.c filesystem/vfs/vfs_mknod.c filesystem/vfs/vfs_mount.c filesystem/vfs/vfs_mp.c filesystem/vfs/vfs_open.c filesystem/vfs/vfs_pipe.c filesystem/vfs/vfs_read.c filesystem/vfs/vfs_rename.c filesystem/vfs/vfs_rmdir.c filesystem/vfs/vfs_umask.c filesystem/vfs/vfs_umount.c filesystem/vfs/vfs_unlink.c filesystem/vfs/vfs_write.c >> .depend
	$(CC) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) filesystem/sfs/sfs_init.c filesystem/sfs/sfs_fileblock.c filesystem/sfs/sfs_mount.c filesystem/sfs/sfs_open.c filesystem/sfs/sfs_read.c filesystem/sfs/sfs_write.c filesystem/sfs/sfs_inode.c filesystem/sfs/sfs_lseek.c filesystem/sfs/sfs_rename.c filesystem/sfs/sfs_fcntl.c filesystem/sfs/sfs_dup.c filesystem/sfs/sfs_fchmod.c filesystem/sfs/sfs_fchown.c filesystem/sfs/sfs_trunc.c filesystem/sfs/sfs_creat.c filesystem/sfs/sfs_umount.c filesystem/sfs/sfs_close.c filesystem/sfs/sfs_unlink.c filesystem/sfs/sfs_mknod.c filesystem/sfs/sfs_link.c filesystem/sfs/sfs_fstat.c filesystem/sfs/sfs_mkdir.c filesystem/sfs/sfs_rmdir.c >> .depend


# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
