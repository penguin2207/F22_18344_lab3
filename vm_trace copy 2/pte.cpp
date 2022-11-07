#include "pte.h"

pageTableEntry::pageTableEntry(unsigned long phys){  // What does this function do?

  /*Set the PPN of this PTE to the physical address phys*/
  ppn = phys;

}
