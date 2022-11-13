#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <utility>

/* Your TLB should be modeled after the
 * Intel Skylake DATA TLB L1 with the following
 * parameters
*/
#define TLB_CACHE_SIZE      1024
#define TLB_LINES           32

#define TLB_SET             16
#define TLB_OFFSET_LEN      12
#define TLB_INDEX_LEN       4
#define TLB_INDEX_MASK      0xF

typedef struct {
    unsigned long set_b; /* set bits */
    unsigned long tag_b; /* tag bits */
    unsigned long off_b; /* offset bits */
    unsigned long vpn;
    int lru;
    bool valid; /* valid bit */
} block_t;

class TLB
{
private:
  size_t tlbSize;

  
  

  size_t block_size;
  size_t sets;
  size_t set_bits;
  size_t lines;
  size_t block_bits;
  size_t tag_bits;

  size_t offMask;
  size_t setMask;
  size_t tagMask;

  block_t **cache;

public:
  size_t misses;
  size_t evictions;
  TLB();
  bool lookup(unsigned long addr, unsigned long & PPN);
  void update(unsigned long addr, unsigned long new_PPN);

};
