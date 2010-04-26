// int vfs_open_dev(u_int16_t major, u_int16_t minor,u_int32_t mode, u_int32_t flags) {
//   
//    // find a file descriptor
//    int i = 0; 
//    while((i<NUM_FD)&&(fdesc[i].in_use)) 
//      i++; 
//    if(i==NUM_FD) 
//      { 
//        // errno = ENFD; no file descriptors 
//        return -1; 
//      } 
//    fdesc[i].in_use = 1; 
// 
//    fdesc[i].mp = NULL; 
//    // fdesc[i].sb = NULL; 
//    // fdesc[i].inode = NULL; 
//    fdesc[i].flags = flags; 
//    fdesc[i].mode = mode; 
//    fdesc[i].major = major; 
//    fdesc[i].minor = minor; 
//    fdesc[i].buffer = NULL; 
//    fdesc[i].bufsize = 0; 
//    fdesc[i].dirty = 0; 
//    fdesc[i].curr_blk = 0; 
//    fdesc[i].curr_log = 0; 
//    fdesc[i].bufpos = 0; 
//    fdesc[i].filepos = 0; 
//    fdesc[i].type = FT_CHAR_SPEC; 
//    return i; 
// 
//  } 
// 
// // COMMENTS 04.22.10
// // function vfs_open_dev should be used
// // to create system file descriptors referring to 
// // the console device. 
// // i.e. during process setup, this function should
// // initialize the first three file descriptors to
// // refer to the console device, up to NUM_FD,
// // the maximum number of file descriptors allocated to
// // each process.