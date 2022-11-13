#include <assert.h>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ptab.h"
#include "pte.h"
#include "pte-util.h"
#include "vm-util.h"

// Function that creates a page table?
pageTable::pageTable(){

  /*Core i7 has 9 bits worth of entries per page table*/
  pageTableIndexBits = VM_PTABBITS;  

  /*Core i7 has 512 entries per page table*/
  pageTableSize = 1 << VM_PTABBITS; 

  /*Core i7 has 4 levels of PTs each with 512 entries*/
  levels = VM_PTABLEVS; 

  /*First level page table gets created and zeroed to start*/
  table = (PTE *)calloc( pageTableSize, sizeof( PTE ) );

}

PTE pageTable::createEntry(unsigned long addr, size_t level){   

  PTE pte;
  pte.pt = (pageTable *)0x0;

<<<<<<< HEAD
  if (level != VM_PTABLEVS-1) {
=======
  if (level < VM_PTABLEVS-1) {
>>>>>>> master
    pte.pt = new pageTable();
  }
  else {
    pte.pte = new pageTableEntry(VM_PAGEDOUT); 
  }
<<<<<<< HEAD
  int idx = getEntryIdFromAddr(addr, level+1);
=======
  unsigned long idx = getEntryIdFromAddr(addr, level+1);
>>>>>>> master
  this->table[idx] = pte;
  
  return pte;

}

PTE pageTable::getEntryDirect(unsigned long index){

<<<<<<< HEAD
  return table[index];            // Does it refer to the object?
=======
  return table[index];
>>>>>>> master

}

PTE pageTable::getEntry(unsigned long addr, size_t level){

  /*TODO: extract the bitfield from the address that corresponds to
          the page table index for this level's page table and return
          the entry in this level's table at that index.

          See also: getEntryIdFromAddr(addr, level)
  */
<<<<<<< HEAD
  PTE pte;
  pte.pt = (pageTable *)0x0;     // Need this just to have it being able to compile

  // PTE *curr_table;
  // unsigned int pt_idx;

  // for(unsigned int i; i < level; i++) {
  // // loop until reached the level we want
  //   pt_idx = getEntryIdFromAddr(addr, i);
  //   curr_table = pte.pt->table;
  //   pte = curr_table[pt_idx];   // Update the pte what we are currently referring to 
  // }


  unsigned int idx;
  idx = getEntryIdFromAddr(addr, level+1);
  pte = this->table[idx];

  return pte;
=======

  unsigned long pt_idx = getEntryIdFromAddr(addr, level+1);

  return this->table[pt_idx];
>>>>>>> master


}

/* Input:  unmodified address.  
   Return: level-specific bit field from address that is index into 
           that level's page table
*/
unsigned pageTable::getEntryIdFromAddr(unsigned long addr, size_t level){
 
  /*TODO: Use the level and the size of the page table to figure out which 
          bits from the address to extract and return.  This bit field from
          the address is the index into that level of page table

          A good chec to be sure that you've extracted the bits correctly
          is to have something like the following assertion in your code...
          assert( (addr & mask) <= pageTableSize );
  */
  int level_shift = VM_PTABLEVS - level;
  int mask = 0x1FF;
  int pte_idx = (addr >> ((VM_PTABBITS*level_shift) + VM_PPOBITS)) & mask;

  assert( (addr & mask) <= pageTableSize );

<<<<<<< HEAD
=======
  size_t level_shift = VM_PTABLEVS-level;
  unsigned long level_mask = 0x1FF;
  unsigned long pte_idx = (addr >> ((VM_PTABBITS*level_shift) + VM_PPOBITS)) & level_mask;

  assert( (addr & level_mask) <= pageTableSize );


>>>>>>> master
  return pte_idx; 

}
 
