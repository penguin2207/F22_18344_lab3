#include <iostream>
#include "vm.h"
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[]){

  vm = new VM();
  void *p = (void*)0x0; 

  /* Test case 1: Pagefault and Segfault */

  // p = (void*)0x0; 
  // vm->vmMap((unsigned long)p,8192);
  
  // std::cerr << "[TEST1: OK; page fault]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+0,0xfeefee);
  
  // std::cerr << "[TEST2: OK; ]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+0,0xfeefee);

  // std::cerr << "[TEST3: OK; ]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+9,0xfeefee);
  
  // std::cerr << "[TEST4: OK; ]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+4095,0xfeefee);
  
  // std::cerr << "[TEST5: OK; page fault]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+4096,0xfeefee);
  
  // std::cerr << "[TEST6: OK; ]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+4096,0xfeefee);
  
  // std::cerr << "[TEST7: OK; ]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+4097,0xfeefee);
  
  // std::cerr << "[TEST8: OK; ]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+5097,0xfeefee);

  // std::cerr << "[TEST: SEGFAULT; ]" << std::endl;
  // vm->pageStoreSingle((unsigned long)p+8193,0xfeefee);


  /* Test case 2: Pagefault consistencies */
  // p = (void*)0x18000; 
  // vm->vmMap((unsigned long)p,65536);
  // for(int i = 0; i < 65536; i+=1024){
  //   vm->pageStoreSingle((unsigned long)p+i,0xfeefee);
  // }


  p = (void*)0x20000;
  vm->vmMap((unsigned long)p,4097);

  //vm->pageStoreMulti((unsigned long)p, 4096,0xfeefee);

  // vm->pageStoreMulti((unsigned long)p, 4096,0xfeefee);  // No segfaults, 1 page fault (1 miss)
  // // vm->pageStoreMulti((unsigned long)p, 4097,0xfeefee);    // One segfault, 1 page fault (1 miss)
  // // vm->pageStoreMulti((unsigned long)p+4096, 2,0xfeefee);  // 2 segfaults

  // vm->pageStoreMulti((unsigned long)p+4095, 2, 0xfeefee);  // no segfault, one pagefault (1 miss) - overflowing to entries
  // vm->pageStoreSingle((unsigned long)p+4097, 0xfeefee);   // hit


  /* Test case 3: Overflowing last level page */
  // p = (void *)0x1FF000;                                     // last index entry of the last level
  // vm->vmMap((unsigned long)p, 8192);                        // Map 8192 (This will create 2 PNN entries, but at different tables)
  // vm->pageStoreMulti((unsigned long)p, 4096, 0xfeefee);     // 1 miss (1 page fault), 4095 hits, 4 accesses
  // vm->pageStoreMulti((unsigned long)p, 4097, 0xfeefee);     // 2 misses (2 page fault), 4095 hits, 8 accesses

  /* Test case 4: Mapping 0 */
  // p = (void*)0x22000;
  // vm->vmMap((unsigned long)p,0);
  // vm->pageStoreMulti((unsigned long)p, 5, 0xfeefee);            // 5 segfaults

  /* Test case 5: Locality */
  p = (void*)0x0;
  vm->vmMap((unsigned long)p, 2097152);   // Map full table
  for (int i = 0; i < 2097152; i++){
      vm->pageStoreSingle((unsigned long)p+i,0xfeefee);     // miss occur at every first access of the entries -> total of 512 misses
  }


  unsigned accesses = vm->getNumAcc();
  unsigned TLB_hits = vm->getNumTlbHit();
  unsigned TLB_misses = vm->getNumTlbMiss();
  unsigned page_faults = vm->getNumPgFault();
  unsigned segfaults = vm->getNumSegfault();

  printf("accesses = %d \n", accesses);
  printf("TLB_hits = %d \n", TLB_hits);
  printf("TLB_misses = %d \n", TLB_misses);
  printf("Page faults = %d \n", page_faults);
  printf("Segfaults = %d \n", segfaults);


  std::cout << "[COUT STATS:]" << std::endl;
  std::cout << "number of virtual memory accesses: " << accesses << std::endl;
  std::cout << "number of TLB hits: " << TLB_hits << std::endl;
  std::cout << "number of TLB misses: " << TLB_misses << std::endl;
  std::cout << "number of page faults: " << page_faults << std::endl;
  std::cout << "number of segfaults: " << segfaults << std::endl;

  std::cerr << "[CERR STATS:]" << std::endl;
  std::cerr << "number of virtual memory accesses: " << accesses << std::endl;
  std::cerr << "number of TLB hits: " << TLB_hits << std::endl;
  std::cout << "number of TLB misses: " << TLB_misses << std::endl;
  std::cerr << "number of page faults: " << page_faults << std::endl;
  std::cerr << "number of segfaults: " << segfaults << std::endl;

}
