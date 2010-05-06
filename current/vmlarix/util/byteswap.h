
#ifndef BYTESWAP_H
#define BYTESWAP_H

#ifdef HOST_IS_BIG_ENDIAN
void byteswap16(void *t);
void byteswap32(void *t);
void byteswap64(void *t);
#else
#define byteswap16(a)
#define byteswap32(a)
#define byteswap64(a)
#endif

#endif
