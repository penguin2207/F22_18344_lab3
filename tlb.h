#pragma once

/* Your TLB should be modeled after the
 * Intel Skylake DATA TLB L1 with the following
 * parameters
*/
#define TLB_SET             16
#define TLB_OFFSET_LEN      12
#define TLB_INDEX_LEN       4
#define TLB_INDEX_MASK      0xF

<<<<<<< Updated upstream
=======
typedef struct {
    // unsigned long set_b; /* set bits */
    unsigned long tag; /* tag bits */
    // unsigned long off_b; /* offset bits */
    // unsigned long vpn;
    unsigned long ppn;
    int lru;
    bool valid; /* valid bit */
} block_t;

>>>>>>> Stashed changes
class TLB
{
private:

public:
  TLB();
  bool lookup(unsigned long addr, unsigned long & PPN);
  void update(unsigned long addr, unsigned long new_PPN);

};
