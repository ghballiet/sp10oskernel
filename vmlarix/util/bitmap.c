
#include "bitmap.h"


/* set a specific bit within a uint32_t */
void set_bit_word(bitmap_t *word, uint32_t bit) {
  *word = (1<<bit) | *word;
}

/* clear a specific bit within a uint32_t */
void clear_bit_word(bitmap_t *word, uint32_t bit) {
  *word = ~(1<<bit) & *word;
}
 
/* get the value of a specific bit within a uint32_t */
uint32_t get_bit_word(bitmap_t word, uint32_t bit) {
  return (1 && (word & (1<<bit)));
}

/* set a bit in the bitmap */
void set_bit(bitmap_t bitmap[], uint32_t bit) {
  /* find out which word the bit is in */
  int i = bit / UNIT_BITS;
  int b = bit % UNIT_BITS;

  /* set the correct bit within that word */
  set_bit_word(&bitmap[i], b);
}

/* clear a bit in the bitmap */
void clear_bit(bitmap_t bitmap[], uint32_t bit) {
  /* find out which word the bit is in */
  int i = bit / UNIT_BITS;
  int b = bit % UNIT_BITS;

  /* clear the correct bit within that word */
  clear_bit_word(&bitmap[i], b);
}

/* get the value of a specific bit within a uint32_t */
uint32_t get_bit(bitmap_t bitmap[], uint32_t bit) {
  /* find out which word the bit is in */
  int i = bit / UNIT_BITS;
  int b = bit % UNIT_BITS;

  /* return the correct bit within that word */
  return get_bit_word(bitmap[i], b);
}