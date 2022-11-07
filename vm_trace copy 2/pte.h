#pragma once

class pageTableEntry{

public:
  unsigned long ppn;  
  pageTableEntry(unsigned long);

  // Added 
  // unsigned long vpn;
  unsigned int perm;   // How many permission mode/flag do we have


};
