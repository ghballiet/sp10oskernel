
#include <bitmap.h>
#include <kprintf.h>

/* set a specific bit within a uint32_t */
void set_bit_word(bitmap_t *word, uint32_t bit)
{
  bitmap_t mask = 1;
  mask <<= bit; 
  *word |= mask;
}

/* clear a specific bit within a uint32_t */
void clear_bit_word(bitmap_t *word, uint32_t bit)
{
  bitmap_t mask = 1;
  mask <<= bit; 
  *word &= ~mask;
}

/* get the value of a specific bit within a uint32_t */
uint32_t get_bit_word(bitmap_t word, uint32_t bit)
{
  bitmap_t mask = 1;
  mask <<= bit; 
  return (((word & mask)>>bit)==1);
}

/* set a bit in the bitmap */
void set_bit(bitmap_t *bitmap, uint32_t bit)
{
  /* find out which word the bit is in */
  uint32_t w = (bit>>BITMAP_SHIFT);
  uint32_t b = (bit&((1<<BITMAP_SHIFT)-1));
  /* set the correct bit within that word */
  set_bit_word(bitmap+w,b);
}

/* clear a bit in the bitmap */
void clear_bit(bitmap_t *bitmap, uint32_t bit)
{
  /* find out which word the bit is in */
  uint32_t w = (bit>>BITMAP_SHIFT);
  uint32_t b = (bit&((1<<BITMAP_SHIFT)-1));
  /* clear the correct bit within that word */
  clear_bit_word(bitmap+w,b);
}

/* get the value of a specific bit within a uint32_t */
uint32_t get_bit(bitmap_t *bitmap, uint32_t bit)
{
  /* find out which word the bit is in */
  uint32_t w = (bit>>BITMAP_SHIFT);
  uint32_t b = (bit&((1<<BITMAP_SHIFT)-1));
  /* return the correct bit within that word */
  return get_bit_word(bitmap[w],b);
}

/* get the index of the first zero bit in the bitmap */
int32_t first_cleared(bitmap_t *bitmap, uint32_t size)
{
  int i=0;
  while((i<size)&&get_bit(bitmap,i))
    i++;
  if(i>=size)
    return -1;
  return i;
}

