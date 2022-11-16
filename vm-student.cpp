#include "vm.h"
#include "vm-util.h"
#include "pte-util.h"
#include "pte.h"
#include "ptab.h"
#include "tlb.h"

#include <assert.h>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <cstddef>
#include <stdio.h>

VM *vm;

/*
 * Task:
 * Implement vmPageFault Handler.
 *
 * Arguments:
 * pageTableEntry *pte from pte.h
 *
 * Requires: 
 * bumpAllocate() from vm-provided.cpp
 *    [returns allocated physical address or 0x0 if must replace page]
 * replacePage() from vm-provided.cpp
 *    [returns physical address of page after replacement]
 *
 * Behavior:
 * Update physical page number in pte to one returned by allocation or
 * replacement
 */
void VM::vmPageFaultHandler(pageTableEntry *pte){

  /*TODO: Attempt to allocate a new physical page
          If all physical pages are occupied, replace
          a page.
          Update the PTE to store the new translation
  */
  
  /* Attempt to allocate a new page */
  unsigned long ppn;
  unsigned long phys_addr = bumpAllocate();

  /* Case1 : Must replace page*/
  if (phys_addr == 0x0)  {

    ppn = 0xdeadbeef;
    while (ppn == 0xdeadbeef) {
      std::pair<unsigned long, unsigned long> replace = replacePage();
      ppn = replace.second;
    }

    //printf("this is ppn %x\n", ppn);
    pte->ppn = ppn; // Update the PTE 
  }

  /* Case 2: Allocation successful*/
  else {
    pte->ppn = (phys_addr & VM_PPNMASK) >> 12;
  }

  _page_faults++; /*Don't forget to update the page fault counter*/
  //assert(false && "vmPageFaultHandler not implemented");
}

/*
 * Task:
 * Implement vmMap
 *
 * Arguments: 
 * vaddr - virtual address to map
 * size - number of bytes in region to map
 *
 * Requires:
 * VM_PPOBITS - number of bits in the physical page offset 
 *
 * Behavior:
 * Updates the page table to include a mappings
 * for size bytes at address vaddr (which may require
 * mapping multiple pages, creating page tables/ptes if they don't exist)
 * 
 */
void VM::vmMap(unsigned long vaddr, size_t size){

  std::cerr << "[VM: Mapping Region " << std::hex << vaddr << " " << std::dec << size << "B]" << std::endl;

  int32_t size_count = size;
  size_t entry_size = 1<<VM_PPOBITS;
  size_t curr_addr = vaddr;
  
  pageTable pT = *(root.pt);
  PTE pte;
  pte.pt = (pageTable *)0x0; 
  int _createPPN = 0;

  printf("Begin mapping %d bytes starting at vaddr: 0x%x\n", (int)size, (int)vaddr);

  while(size_count>0){
    //Use curr_addr to check for existing entry/create
    
    for (int i  = 0; i < (int) levels; i++) {
      pte = pT.getEntry(curr_addr, i);
      if (i < (int)(levels-1)) {
        if (!pte.pt) {
          pte.pt=pT.createEntry(curr_addr, i).pt;
        }
        else if ((unsigned long)(pte.pt) == VM_PAGEDOUT) {
          pte.pt=pT.createEntry(curr_addr, i).pt;
        }
        pT = *pte.pt;
        // printf("pT address starts at %p for level %d\n", (void *)(pte.pt), i+2);
      } else {
        
        if(!pte.pte){
          pte.pte = pT.createEntry(curr_addr, i).pte;
          _createPPN++;
        }
        if (pte.pte->ppn == (unsigned long)0x0) {  // In the unmapped case <not happening in our simulation>
          pte.pte->ppn = VM_PAGEDOUT;
        }
      }
         
    }
    pT = *(root.pt);
    size_count-=entry_size;
    curr_addr+=entry_size;
    // printf("%d bytes left to be mapped \n", size_count);
  }
  // printf("Create PPN: %d\n", _createPPN);
  
  /*TODO: Compute the number of pages in the region to be mapped
          Create an entry in the page table for each page to be mapped 
  */
  //assert(false && "Abort: vmMap() unimplemented");
}

/*
 * Task:
 * Implement vmTranslate
 *
 * Arguments: 
 * vaddr - virtual address to translate 
 *
 * Requires:
 * VM_PPOMASK - physical page offset mask
 * VM_PPOBITS - number of bits in the physical page offset 
 * VM_PAGEDOUT - a page is paged out if its ppn == VM_PAGEDOUT
 * addToReplacementList() - add a replaced page to the replacement list
 *
 * Behavior:
 * Translate the virtual address addr to its corresponding
 * physical address (i.e., using the PPN and PPO), 
 * checking the TLB, if implemented,
 * then checking the page table and handling page faults.
 * Returns the translated physical address
 *
 */
unsigned long VM::vmTranslate(unsigned long addr){

  /*TODO: *Extract the PPO from the virtual address
          *Check your TLB for a cached translation if you have a TLB 
          *Starting from the root page table address
          *Traverse the page table to get the Entry for this address 
          *If paged out, handle page fault, add addr to replacement list
          *Update TLB to cache new translation if you have a TLB 
          *Assemble and return physical address
  */

  // Extract PPO
  unsigned long VPO, PPN, PPO, refPPN;
  //(VPN, VPO) = (addr[63:12], addr[11:0]);
  // unsigned long VM_VPNMASK = 0xFFFFFF;
  // VPN = (addr >> 12) & VM_VPNMASK;
  VPO = addr & VM_PPOMASK;
  PPO = VPO; 

  pageTable pt = *(root.pt);
  pageTableEntry *ppn_table;
  PTE pte;
  unsigned long phys_addr;
  pte.pt = (pageTable *)0x0; 
  // bool tlbHit = false;

   

  if(_TLB->lookup(addr, refPPN)) {
    // tlbHit = true;
    _tlb_hits++;
    return refPPN;
  }

  
  /* We can ignore this case */
  // if (&root == NULL || root.pt == NULL) {
  //   _page_faults++;
  // }

  for (unsigned int i  = 0; i < levels; i++) {  // Traversing up to level 3
  _accesses++; /*Don't forget to update the access counter*/
    pte = pt.getEntry(addr, i); // Level in our function is 1-4
    if (i != levels - 1) {
      if (pte.pt == NULL) {
        _segfaults++; // segfault
        return -1;
      }
      else if ((unsigned long)(pte.pt) == VM_PAGEDOUT) {
        /* This case would not happen in the simulation */
        _page_faults++;
        return -1;
      }
      pt = *pte.pt;  //the next level page table
    }
    else { // when i = 3 (level = 4)
      ppn_table = pte.pte;
    }
  }

  if (ppn_table == NULL) {
    _segfaults++; // segfault
    return -1;
  }
  PPN = ppn_table->ppn;
  
  // Check PPN
  if (PPN == VM_PAGEDOUT) { // Last level paged out -> Page In 
    // Page In 
    vmPageFaultHandler(ppn_table);
    // _page_faults++; // already counted in the faultHandler
    PPN = ppn_table->ppn;
    phys_addr = (PPN << VM_PPOBITS) || PPO;
    addToReplacementList(addr);

  }
  else if (PPN == (unsigned long)0x0) {  // Unmapped <not happening in our simulation>
    /* In real life we call segfault */
    _segfaults++;
    return -1;
  }
  // Else -- already exist
  phys_addr = (PPN << VM_PPOBITS) || PPO;

  // if((tlbHit) && (refPPN == phys_addr))
  //   _tlb_hits++;
  // else {
  //   _tlb_misses++;
  //   _TLB->update(addr, phys_addr);
  // }
  _tlb_misses++;
  _TLB->update(addr, phys_addr);

  //assert(false && "Abort: vmTranslate not implemented");
  return phys_addr;
}


void VM::Load(unsigned long addr){

  unsigned long translatedPaddr = vmTranslate(addr);
  std::cerr << "Load[0x" << std::hex << addr << 
                    "] => [" << translatedPaddr << "]" << std::endl;

}


void VM::Store(unsigned long addr){

  unsigned long translatedPaddr = vmTranslate(addr);
  // std::cerr << "Store[0x" << std::hex << addr << 
  //                   "] => [" << translatedPaddr << "]" << std::endl;

}
