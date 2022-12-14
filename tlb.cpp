#include "tlb.h"
#include "vm.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

TLB::TLB()
{
  misses = 0;
  evictions = 0;
  sets = TLB_SET;
  
  lines = (size_t)((TLB_CACHE_SIZE / TLB_BLOCKSIZE) / TLB_SET);
  block_bits = TLB_BLOCKBITS;
  

  offMask = TLB_BLOCKSIZE - 1;
  setMask = sets - 1;

  cache = (block_t **)malloc((sets) *
                                 sizeof(block_t *));
    for (int i = 0; i < (int)sets; ++i) {
        cache[i] =
            (block_t *)malloc(((unsigned long long)lines) * sizeof(block_t));
    }
    // Initialize tlb values
    for (int i = 0; i < (int)sets; ++i) {
        for (int j = 0; j < (int)lines; ++j) {
            cache[i][j].valid = false;
            cache[i][j].ppn = 0;
            cache[i][j].tag = 0;
        }
    }

}

bool TLB::lookup(unsigned long addr, unsigned long &PPN){

  /*TODO: Implement a TLB lookup here, assign the PPN ref arg
          to the PPN cached in the TLB, and return true if
          your TLB lookup is a hit
  */
  unsigned long tagBits = (addr & VM_PPNMASK) >> 12;
  unsigned int setBits = ((addr >> VM_PPOBITS) & setMask);

  for (int i = 0; i < (int)lines; ++i) {
        if (cache[setBits][i].valid &&
            (cache[setBits][i].tag == tagBits)) {
            PPN = cache[setBits][i].ppn;
            return true;
        }
  }

  return false;
}

void TLB::update(unsigned long addr, unsigned long new_PPN){
  
  /* TODO: Implement a TLB update here using the 
          virtual address and new PPN */

  unsigned int setBits = ((addr >> VM_PPOBITS) & setMask);
  unsigned long tagBits = (addr & VM_PPNMASK) >> 12;

  size_t randLine = rand() % lines;
  cache[setBits][randLine].ppn = new_PPN;
  cache[setBits][randLine].valid = true; 
  cache[setBits][randLine].tag = tagBits;
  if(cache[setBits][randLine].valid)
    evictions++;
  misses++;
  return;
}