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
  
  lines = TLB_LINES/TLB_SET; // associativty
  block_size = (size_t)((TLB_CACHE_SIZE / TLB_LINES) / TLB_SET);
  block_bits = (size_t)(log((double)(block_size))/log((double)2));// (cachesize / linesize)
  

  offMask = block_size - 1;
  setMask = sets - 1;

  cache = (block_t **)malloc((sets) *
                                 sizeof(block_t *));
    for (int i = 0; i < (int)sets; ++i) {
        cache[i] =
            (block_t *)malloc(((unsigned long long)lines) * sizeof(block_t));
    }
    // Initialize cache values
    for (int i = 0; i < (int)sets; ++i) {
        for (int j = 0; j < (int)lines; ++j) {
            cache[i][j].valid = false;
            cache[i][j].lru = 0;
            // cache[i][j].set_b = 0;
            // cache[i][j].off_b = 0;
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

  unsigned long setBits = (addr & (setMask << block_bits)) >> block_bits;
  unsigned long tagMask = 0xFFFFFFFF << (set_bits + block_bits);
  unsigned long tagBits = (addr & tagMask) >> (set_bits + block_bits);

  for (int i = 0; i < (int)lines; ++i) {
        if (cache[setBits][i].valid &&
            (cache[setBits][i].tag == tagBits)) {  // check the cache tag
            PPN = cache[setBits][i].ppn;
            return true;
        }
  }

  return false;

}

void TLB::update(unsigned long addr, unsigned long new_PPN){
  
  /*TODO: Implement a TLB update here using the 
          virtual address and new PPN*/

   unsigned long setBits = (addr & (setMask << block_bits)) >> block_bits;
  unsigned long offBits = addr & offMask;
  unsigned long tagMask = 0xFFFFFFFF << (set_bits + block_bits);

  // unsigned long tagBits = new_PPN;
  unsigned long tagBits = (addr & tagMask) >> (set_bits + block_bits);

  // for (int i = 0; i < (int)lines; ++i) {
  //       if (cache[setBits][i].valid &&
  //           (cache[setBits][i].vpn == (addr & VM_PPNMASK))) {
  //           cache[setBits][i].tag_b = new_PPN;
  //           misses++;
  //           return;
  //       }
  // }

  size_t randLine = rand() % lines;
  // cache[setBits][randLine].set_b = setBits;
  cache[setBits][randLine].ppn = new_PPN;
  // cache[setBits][randLine].off_b = offBits;
  cache[setBits][randLine].valid = true; 
  // cache[setBits][randLine].vpn = (addr & VM_PPNMASK);
  cache[setBits][randLine].tag = tagBits;
  if(cache[setBits][randLine].valid)   // sI don't think we have to check this becuase we are not store the values in this to memeory
    evictions++;
  misses++;
  return;

}
