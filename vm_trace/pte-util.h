#pragma once
class pageTable;
class pageTableEntry;
typedef union pte_t{       // Question: why union?

  pageTable *pt;                
  pageTableEntry *pte;

} PTE;
