rm -f initrd
./create_ramdisk initrd 128 8192
./mkfs_sfs initrd
./cp_to_rd create_ramdisk.c initrd
