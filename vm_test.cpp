#include <iostream>
#include "vm.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

using namespace std;

void StoreMulti(VM *vm, unsigned long addr, uint32_t size, uint32_t instId){
  for (uint32_t i = 0; i < size; i++){
    vm->Store(addr);
    addr++;
  }
  
}

void testCase1(){

  vm = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt");
 
  void *p = (void*)0x0; 
  vm->vmMap((unsigned long)p,8192);

  /* Test case 1: Pagefault and Segfault */
  
  std::cerr << "[TEST1: OK; page fault]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+0,0xfeefee);
  
  std::cerr << "[TEST2: OK; ]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+0,0xfeefee);

  std::cerr << "[TEST3: OK; ]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+9,0xfeefee);
  
  std::cerr << "[TEST4: OK; ]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+4095,0xfeefee);
  
  std::cerr << "[TEST5: OK; page fault]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+4096,0xfeefee);
  
  std::cerr << "[TEST6: OK; ]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+4096,0xfeefee);
  
  std::cerr << "[TEST7: OK; ]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+4097,0xfeefee);
  
  std::cerr << "[TEST8: OK; ]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+5097,0xfeefee);

  std::cerr << "[TEST: SEGFAULT; ]" << std::endl;
  vm->pageStoreSingle((unsigned long)p+8193,0xfeefee);

  unsigned accesses1 = vm->getNumAcc();
  unsigned TLB_hits1 = vm->getNumTlbHit();
  unsigned TLB_misses1 = vm->getNumTlbMiss();
  unsigned page_faults1 = vm->getNumPgFault();
  unsigned segfaults1 = vm->getNumSegfault();


  results << "Test 1, TLB \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";

  results.close();

}

void testCase2(){
  /* Test case 2: Pagefault consistencies */

  vm = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  void *p = (void*)0x18000; 
  vm->vmMap((unsigned long)p,65536);
  for(int i = 0; i < 65536; i+=1024){
    vm->pageStoreSingle((unsigned long)p+i,0xfeefee);
  }

  unsigned accesses1 = vm->getNumAcc();
  unsigned TLB_hits1 = vm->getNumTlbHit();
  unsigned TLB_misses1 = vm->getNumTlbMiss();
  unsigned page_faults1 = vm->getNumPgFault();
  unsigned segfaults1 = vm->getNumSegfault();


  results << "Test 2, TLB \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";


  
}

void testCase3(){

  /* Test case 3: TLB High Hit Rate */

  vm = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  void *p = (void*)0xff9000; 
  vm->vmMap((unsigned long)p,65536);
  for(int i = 0; i < 65536; i+=1024){
    vm->pageStoreSingle((unsigned long)p+i,1024);
  }

  for(int i = 1; i < 65537; i+=1024){
    vm->pageStoreSingle((unsigned long)p+i,1024);
  }
  unsigned accesses1 = vm->getNumAcc();
  unsigned TLB_hits1 = vm->getNumTlbHit();
  unsigned TLB_misses1 = vm->getNumTlbMiss();
  unsigned page_faults1 = vm->getNumPgFault();
  unsigned segfaults1 = vm->getNumSegfault();


  results << "Test 3, TLB \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
  
}

void testCase4(){

  /* Test case 4: Overflowing last level page */
  vm = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  void *p = (void *)0x1FF000;                                     // last index entry of the last level
  vm->vmMap((unsigned long)p, 8192);                        // Map 8192 (This will create 2 PNN entries, but at different tables)
  StoreMulti(vm, (unsigned long)p, 4096, 0xfeefee);     // 1 miss (1 page fault), 4095 hits, 4 accesses
  StoreMulti(vm, (unsigned long)p, 4097, 0xfeefee);     // 2 misses (2 page fault), 4095 hits, 8 accesses
  unsigned accesses1 = vm->getNumAcc();
  unsigned TLB_hits1 = vm->getNumTlbHit();
  unsigned TLB_misses1 = vm->getNumTlbMiss();
  unsigned page_faults1 = vm->getNumPgFault();
  unsigned segfaults1 = vm->getNumSegfault();


  results << "Test 4, TLB \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
}

void testCase5(){
  /* Test case 5: Locality */
  vm = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  void *p = (void*)0x0;
  vm->vmMap((unsigned long)p, 2097152);   // Map full table
  for (int i = 0; i < 2097152; i++){
      vm->pageStoreSingle((unsigned long)p+i,0xfeefee);     // miss occur at every first access of the entries -> total of 512 misses
  }
  unsigned accesses1 = vm->getNumAcc();
  unsigned TLB_hits1 = vm->getNumTlbHit();
  unsigned TLB_misses1 = vm->getNumTlbMiss();
  unsigned page_faults1 = vm->getNumPgFault();
  unsigned segfaults1 = vm->getNumSegfault();


  results << "Test 5, TLB \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
  
}

void testCase6(){
  vm = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  /* Test case 6: Mapping 0 */
  void *p = (void*)0x22000;
  vm->vmMap((unsigned long)p,0);
  StoreMulti(vm, (unsigned long)p, 5, 0xfeefee);            // 5 segfaults
  unsigned accesses1 = vm->getNumAcc();
  unsigned TLB_hits1 = vm->getNumTlbHit();
  unsigned TLB_misses1 = vm->getNumTlbMiss();
  unsigned page_faults1 = vm->getNumPgFault();
  unsigned segfaults1 = vm->getNumSegfault();


  results << "Test 6, TLB \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
  
}

void testCase7(){
  vm = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  /* Test case 7: TLB Size Comparison */
  void *p = (void*)0x0; 
  vm->vmMap((unsigned long)p,0xfffff000);
  for(unsigned long i = 0; i < 0xfffff000; i+=0x1000){
    StoreMulti(vm, (unsigned long)p+i,4096,0xfeefee);
  }

  for(unsigned long i = 0x7ffff800; i < 0xfffff000; i+=0x2000){
    StoreMulti(vm, (unsigned long)p+i,4096,0xfeefee);
  }

  unsigned accesses1 = vm->getNumAcc();
  unsigned TLB_hits1 = vm->getNumTlbHit();
  unsigned TLB_misses1 = vm->getNumTlbMiss();
  unsigned page_faults1 = vm->getNumPgFault();
  unsigned segfaults1 = vm->getNumSegfault();

  results << "Test 7, TLB \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
  
}

int main(int argc, char *argv[]){

  
  
  testCase1();
  testCase2();
  testCase3();
  testCase4();
  testCase5();
  testCase6();
  testCase7();

  return 0;


  

}