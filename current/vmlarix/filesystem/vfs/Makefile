
MAKE_DIRS= 

C_OBJS = vfs_close.o vfs_creat.o vfs_dup.o vfs_fchmod.o vfs_fchown.o vfs_fcntl.o vfs_filedesc.o vfs_fsops.o vfs_fstat.o vfs_init.o vfs_link.o vfs_lseek.o vfs_mkdir.o vfs_mknod.o vfs_mount.o vfs_mp.o vfs_open.o vfs_pipe.o vfs_read.o vfs_rename.o vfs_rmdir.o vfs_umask.o vfs_umount.o vfs_unlink.o vfs_write.o

ASM_OBJS = 

OBJS = $(C_OBJS) $(ASM_OBJS)

INCLUDES=-I. -I../../util -I../../include -I../../mm -I../../drivers/block -I../../drivers/char

all: ../libvfs.a

../libvfs.a: $(OBJS) 
	$(AR) rcs $@ $(OBJS)

# The following is a "general" recipe for making .o from .s

.s.o:
	$(ASM) $(ASFLAGS) -o $@ $<

# The following is a "general" recipe for making .o from .c

.c.o:
	$(CC) $(DEFINES) $(CFLAGS) $(INCLUDES) $<

# make clean will remove all .o files

clean:
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) clean; cd ..; \
	done
	rm -f *.o ../../libarch.a

# make realclean cleans more aggressively

realclean:
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) realclean; cd ..; \
	done
	rm -f *.o *~ vmlarix
	find . -name .depend -exec rm \{\} \;


# make depend will create a file ".depend" with all the dependencies

depend: 
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) depend; cd ..; \
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
