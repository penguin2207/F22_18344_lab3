#include <iostream>
#include "vm.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

using namespace std;

void StoreMulti(VM *vmP, unsigned long addr, uint32_t size, uint32_t instId){
  for (uint32_t i = 0; i < size; i++){
    vmP->Store(addr);
    addr++;
  }
  
}

void testCase1(){

  VM *vm2 = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt");
 
  void *p = (void*)0x0; 
  vm2->vmMap((unsigned long)p,8192);

  /* Test case 1: Pagefault and Segfault */
  
  std::cerr << "[TEST1: OK; page fault]" << std::endl;
  vm2->Store((unsigned long)p+0);
  
  std::cerr << "[TEST2: OK; ]" << std::endl;
  vm2->Store((unsigned long)p+0);

  std::cerr << "[TEST3: OK; ]" << std::endl;
  vm2->Store((unsigned long)p+9);
  
  std::cerr << "[TEST4: OK; ]" << std::endl;
  vm2->Store((unsigned long)p+4095);
  
  std::cerr << "[TEST5: OK; page fault]" << std::endl;
  vm2->Store((unsigned long)p+4096);
  
  std::cerr << "[TEST6: OK; ]" << std::endl;
  vm2->Store((unsigned long)p+4096);
  
  std::cerr << "[TEST7: OK; ]" << std::endl;
  vm2->Store((unsigned long)p+4097);
  
  std::cerr << "[TEST8: OK; ]" << std::endl;
  vm2->Store((unsigned long)p+5097);

  std::cerr << "[TEST: SEGFAULT; ]" << std::endl;
  vm2->Store((unsigned long)p+8193);

  unsigned accesses1 = vm2->getNumAcc();
  unsigned TLB_hits1 = vm2->getNumTlbHit();
  unsigned TLB_misses1 = vm2->getNumTlbMiss();
  unsigned page_faults1 = vm2->getNumPgFault();
  unsigned segfaults1 = vm2->getNumSegfault();


  results << "Test 1 \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";

  results.close();

}

void testCase2(){
  /* Test case 2: Pagefault consistencies */

  VM *vm2 = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  void *p = (void*)0x18000; 
  vm2->vmMap((unsigned long)p,65536);
  for(int i = 0; i < 65536; i+=1024){
    vm2->Store((unsigned long)p+i);
  }

  unsigned accesses1 = vm2->getNumAcc();
  unsigned TLB_hits1 = vm2->getNumTlbHit();
  unsigned TLB_misses1 = vm2->getNumTlbMiss();
  unsigned page_faults1 = vm2->getNumPgFault();
  unsigned segfaults1 = vm2->getNumSegfault();


  results << "Test 2 \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";


  
}

void testCase3(){

  /* Test case 3: Overflowing last level page */
  VM *vm2 = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  void *p = (void *)0x1FF000;                                     // last index entry of the last level
  vm2->vmMap((unsigned long)p, 8192);                        // Map 8192 (This will create 2 PNN entries, but at different tables)
  StoreMulti(vm2, (unsigned long)p, 4096, 0xfeefee);     // 1 miss (1 page fault), 4095 hits, 4 accesses
  StoreMulti(vm2, (unsigned long)p, 4097, 0xfeefee);     // 2 misses (2 page fault), 4095 hits, 8 accesses
  unsigned accesses1 = vm2->getNumAcc();
  unsigned TLB_hits1 = vm2->getNumTlbHit();
  unsigned TLB_misses1 = vm2->getNumTlbMiss();
  unsigned page_faults1 = vm2->getNumPgFault();
  unsigned segfaults1 = vm2->getNumSegfault();


  results << "Test 3 \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
}

void testCase4(){
  VM *vm2 = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  /* Test case 4: Mapping 0 */
  void *p = (void*)0x22000;
  vm2->vmMap((unsigned long)p,0);
  StoreMulti(vm2, (unsigned long)p, 5, 0xfeefee);            // 5 segfaults
  unsigned accesses1 = vm2->getNumAcc();
  unsigned TLB_hits1 = vm2->getNumTlbHit();
  unsigned TLB_misses1 = vm2->getNumTlbMiss();
  unsigned page_faults1 = vm2->getNumPgFault();
  unsigned segfaults1 = vm2->getNumSegfault();


  results << "Test 4 \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
  
}

void testCase5(){
  /* Test case 5: Locality */
  VM *vm2 = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  void *p = (void*)0x0;
  vm2->vmMap((unsigned long)p, 2097152);   // Map full table
  for (int i = 0; i < 2097152; i++){
      vm2->Store((unsigned long)p+i);     // miss occur at every first access of the entries -> total of 512 misses
  }
  unsigned accesses1 = vm2->getNumAcc();
  unsigned TLB_hits1 = vm2->getNumTlbHit();
  unsigned TLB_misses1 = vm2->getNumTlbMiss();
  unsigned page_faults1 = vm2->getNumPgFault();
  unsigned segfaults1 = vm2->getNumSegfault();


  results << "Test 5 \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
  
}

void testCase6(){
  VM *vm2 = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  /* Test case 6: Random Store */
  void* p = (void*)0x0;
  vm2->vmMap((unsigned long)p, 2097152);   // Map full table

  for (int i = 0; i < 2097152; i++){
      vm2->Store((unsigned long)p+i);     // miss occur at every first access of the entries -> total of 512 misses
  }

  vm2->Store((unsigned long)p);
  vm2->Store((unsigned long)p+15000);
  vm2->Store((unsigned long)p+5087);
  vm2->Store((unsigned long)p+455087);
  vm2->Store((unsigned long)p+635087);
  vm2->Store((unsigned long)p+1493473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+123473);
  vm2->Store((unsigned long)p+173473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+123473);

  vm2->Store((unsigned long)p+13873);
  vm2->Store((unsigned long)p+3873);
  vm2->Store((unsigned long)p+563872);
  vm2->Store((unsigned long)p+434872);
  vm2->Store((unsigned long)p+1434872);
  vm2->Store((unsigned long)p+1230000);
  vm2->Store((unsigned long)p+1234872);
  vm2->Store((unsigned long)p+1485000);
  vm2->Store((unsigned long)p+80000);
  vm2->Store((unsigned long)p+17000);
  vm2->Store((unsigned long)p+25000);
  vm2->Store((unsigned long)p+250000);
  vm2->Store((unsigned long)p+1687552); 
  vm2->Store((unsigned long)p+1974272); 
  vm2->Store((unsigned long)p+1925120); 
  vm2->Store((unsigned long)p+1892352);
  vm2->Store((unsigned long)p+173473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+900000);
  vm2->Store((unsigned long)p+800000);

  vm2->Store((unsigned long)p+2084864); 
  vm2->Store((unsigned long)p+2048000); 
  vm2->Store((unsigned long)p+2056192); 
  vm2->Store((unsigned long)p+2093056); 

  vm2->Store((unsigned long)p);
  vm2->Store((unsigned long)p+15000);
  vm2->Store((unsigned long)p+5087);
  vm2->Store((unsigned long)p+455087);
  vm2->Store((unsigned long)p+635087);
  vm2->Store((unsigned long)p+1493473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+123473);
  vm2->Store((unsigned long)p+173473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+123473);
  vm2->Store((unsigned long)p+13873);
  vm2->Store((unsigned long)p+3873);
  vm2->Store((unsigned long)p+563872);
  vm2->Store((unsigned long)p+434872);
  vm2->Store((unsigned long)p+1434872);
  vm2->Store((unsigned long)p+1230000);
  vm2->Store((unsigned long)p+1234872);
  vm2->Store((unsigned long)p+1485000);
  vm2->Store((unsigned long)p+80000);
  vm2->Store((unsigned long)p+17000);
  vm2->Store((unsigned long)p+25000);
  vm2->Store((unsigned long)p+250000);
  vm2->Store((unsigned long)p+1687552); 
  vm2->Store((unsigned long)p+1974272); 
  vm2->Store((unsigned long)p+1925120); 
  vm2->Store((unsigned long)p+1892352);
  vm2->Store((unsigned long)p+173473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+900000);
  vm2->Store((unsigned long)p+800000);
  vm2->Store((unsigned long)p+2084864); 
  vm2->Store((unsigned long)p+2048000); 
  vm2->Store((unsigned long)p+2056192); 
  vm2->Store((unsigned long)p+2093056); 
  vm2->Store((unsigned long)p);
  vm2->Store((unsigned long)p+15000);
  vm2->Store((unsigned long)p+5087);
  vm2->Store((unsigned long)p+455087);
  vm2->Store((unsigned long)p+635087);
  vm2->Store((unsigned long)p+1493473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+123473);
  vm2->Store((unsigned long)p+173473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+123473);
  vm2->Store((unsigned long)p+13873);
  vm2->Store((unsigned long)p+3873);
  vm2->Store((unsigned long)p+563872);
  vm2->Store((unsigned long)p+434872);
  vm2->Store((unsigned long)p+1434872);
  vm2->Store((unsigned long)p+1230000);
  vm2->Store((unsigned long)p+1234872);
  vm2->Store((unsigned long)p+1485000);
  vm2->Store((unsigned long)p+80000);
  vm2->Store((unsigned long)p+17000);
  vm2->Store((unsigned long)p+25000);
  vm2->Store((unsigned long)p+250000);
  vm2->Store((unsigned long)p+1687552); 
  vm2->Store((unsigned long)p+1974272); 
  vm2->Store((unsigned long)p+1925120); 
  vm2->Store((unsigned long)p+1892352);
  vm2->Store((unsigned long)p+173473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+900000);
  vm2->Store((unsigned long)p+800000);
  vm2->Store((unsigned long)p+2084864); 
  vm2->Store((unsigned long)p+2048000); 
  vm2->Store((unsigned long)p+2056192); 
  vm2->Store((unsigned long)p+2093056); 
  vm2->Store((unsigned long)p);
  vm2->Store((unsigned long)p+15000);
  vm2->Store((unsigned long)p+5087);
  vm2->Store((unsigned long)p+455087);
  vm2->Store((unsigned long)p+635087);
  vm2->Store((unsigned long)p+1493473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+123473);
  vm2->Store((unsigned long)p+173473);
  vm2->Store((unsigned long)p+293473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+13873);
  vm2->Store((unsigned long)p+3873);
  vm2->Store((unsigned long)p+563872);
  vm2->Store((unsigned long)p+434872);
  vm2->Store((unsigned long)p+1434872);
  vm2->Store((unsigned long)p+1230000);
  vm2->Store((unsigned long)p+1214872);
  vm2->Store((unsigned long)p+1185000);
  vm2->Store((unsigned long)p+30000);
  vm2->Store((unsigned long)p+17000);
  vm2->Store((unsigned long)p+29000);
  vm2->Store((unsigned long)p+250000);
  vm2->Store((unsigned long)p+1787552); 
  vm2->Store((unsigned long)p+1974272); 
  vm2->Store((unsigned long)p+1925120); 
  vm2->Store((unsigned long)p+1892352);
  vm2->Store((unsigned long)p+473473);
  vm2->Store((unsigned long)p+193473);
  vm2->Store((unsigned long)p+900000);
  vm2->Store((unsigned long)p+800000);
  vm2->Store((unsigned long)p+4864); 
  vm2->Store((unsigned long)p+48000); 
  vm2->Store((unsigned long)p+56192); 
  vm2->Store((unsigned long)p+93056); 
  vm2->Store((unsigned long)p+98152); 
  vm2->Store((unsigned long)p+655087);
  vm2->Store((unsigned long)p+655087);
  vm2->Store((unsigned long)p+1283473);
  vm2->Store((unsigned long)p+153473);
  vm2->Store((unsigned long)p+142473);
  vm2->Store((unsigned long)p+97473);
  vm2->Store((unsigned long)p+263473);
  vm2->Store((unsigned long)p+283473);
  vm2->Store((unsigned long)p+290473);
  vm2->Store((unsigned long)p+1686552); 
  vm2->Store((unsigned long)p+1964272); 
  vm2->Store((unsigned long)p+1935120); 
  vm2->Store((unsigned long)p+1852352);

  unsigned accesses1 = vm2->getNumAcc();
  unsigned TLB_hits1 = vm2->getNumTlbHit();
  unsigned TLB_misses1 = vm2->getNumTlbMiss();
  unsigned page_faults1 = vm2->getNumPgFault();
  unsigned segfaults1 = vm2->getNumSegfault();

  results << "Test 6 \n";
  results << "accesses = " << accesses1 << "\n";
  results << "TLB_hits = " << TLB_hits1 << "\n";
  results << "TLB_misses = " << TLB_misses1 << "\n";
  results << "Page faults = " << page_faults1 << "\n";
  results << "Segfaults = " << segfaults1 << "\n";
  
}

void testCase7(){
  VM *vm2 = new VM();

  ofstream results;
  results.open ("./results/w_tlb/results_256.txt", ios::app);

  /* Test case 7: Exhaustive Random Store */
  void* p = (void*)0x0;
  vm2->vmMap((unsigned long)p, 4194304);   // Map full table

  for (int i = 0; i < 4194304; i++){
      vm2->Store((unsigned long)p+i);     // miss occur at every first access of the entries -> total of 512 misses
  }

  int random = 0;
  for (int i = 0; i < 400000; i++){
    random = rand() % 4194304;
    vm2->Store((unsigned long)random);     // miss occur at every first access of the entries -> total of 512 misses
  }
  unsigned accesses1 = vm2->getNumAcc();
  unsigned TLB_hits1 = vm2->getNumTlbHit();
  unsigned TLB_misses1 = vm2->getNumTlbMiss();
  unsigned page_faults1 = vm2->getNumPgFault();
  unsigned segfaults1 = vm2->getNumSegfault();

  results << "Test 7 \n";
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