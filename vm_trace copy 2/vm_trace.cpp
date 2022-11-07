#include "pin.H"

#include <stdio.h>
#include <syscall.h>
#include <iostream>
#include <fstream>
#include <cassert>

using std::string;
using std::cerr;
using std::endl;
std::ofstream out;


int lastSysCall;
bool logAccesses;
/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,    "pintool",
    "o", "vmtrace.out", "specify mmap/access log file name");
KNOB<bool> KnobAccessLog(KNOB_MODE_WRITEONCE,    "pintool",
    "a", "false", "specify whether to log accesses");

/* ===================================================================== */

// Print syscall number and arguments
VOID SysBefore(ADDRINT ip, ADDRINT num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5)
{

    lastSysCall = num;
    if (num == SYS_mmap)
    {

      out << std::hex << "PC=0x" << (unsigned long)ip << ": mmap(vaddr = 0x" << (unsigned long)arg0 << std::dec << " len = " << (unsigned long)arg1 << std::hex << " prot = 0x" << (unsigned long)arg2 << " flags = 0x" << (unsigned long)arg3 << " fd = 0x" << (unsigned long) arg4 << " offset = 0x" << (unsigned long) arg5 << ")";
    }

}

// Print the return value of the system call
VOID SysAfter(ADDRINT ret)
{
    if(lastSysCall == SYS_mmap){ 
      out << "ret = 0x" << (unsigned long)ret << std::endl;
      lastSysCall = -1;
    }
}

VOID SyscallEntry(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
    SysBefore(PIN_GetContextReg(ctxt, REG_INST_PTR),
        PIN_GetSyscallNumber(ctxt, std),
        PIN_GetSyscallArgument(ctxt, std, 0),
        PIN_GetSyscallArgument(ctxt, std, 1),
        PIN_GetSyscallArgument(ctxt, std, 2),
        PIN_GetSyscallArgument(ctxt, std, 3),
        PIN_GetSyscallArgument(ctxt, std, 4),
        PIN_GetSyscallArgument(ctxt, std, 5));
}

VOID SyscallExit(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
    SysAfter(PIN_GetSyscallReturn(ctxt, std));
}



INT32 Usage()
{
    cerr <<
        "This tool represents a cache simulator.\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}


VOID LoadSingle(ADDRINT addr, UINT32 instId)
{
  if(logAccesses){
    out << "ld 0x" << (unsigned long)addr << std::endl;
  }
}

/* ===================================================================== */

VOID StoreSingle(ADDRINT addr, UINT32 instId)
{
  if(logAccesses){
    out << "st 0x" << (unsigned long)addr << std::endl;
  }
}


/* ===================================================================== */

VOID Instruction(INS ins, void * v)
{
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Instrument each memory operand. If the operand is both read and written
    // it will be processed twice.
    // Iterating over memory operands ensures that instructions on IA-32 with
    // two read operands (such as SCAS and CMPS) are correctly handled.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
                // map sparse INS addresses to dense IDs
                const ADDRINT iaddr = INS_Address(ins);

                    INS_InsertPredicatedCall(
                        ins, IPOINT_BEFORE, (AFUNPTR) LoadSingle,
                        IARG_MEMORYOP_EA, memOp,
                        IARG_UINT32, iaddr,
                        IARG_END);
        }
        
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
                const ADDRINT iaddr = INS_Address(ins);

                    INS_InsertPredicatedCall(
                        ins, IPOINT_BEFORE,  (AFUNPTR) StoreSingle,
                        IARG_MEMORYOP_EA, memOp,
                        IARG_UINT32, iaddr,
                        IARG_END);
        }
    }


    if (INS_IsSyscall(ins) && INS_HasFallThrough(ins))
    {
        // Arguments and syscall number is only available before
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SysBefore),
                       IARG_INST_PTR, IARG_SYSCALL_NUMBER,
                       IARG_SYSARG_VALUE, 0, IARG_SYSARG_VALUE, 1,
                       IARG_SYSARG_VALUE, 2, IARG_SYSARG_VALUE, 3,
                       IARG_SYSARG_VALUE, 4, IARG_SYSARG_VALUE, 5,
                       IARG_END);

        // return value only available after
        INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(SysAfter),
                       IARG_SYSRET_VALUE,
                       IARG_END);
    }

}

/* ===================================================================== */

VOID Fini(int code, VOID * v)
{

    //std::ofstream out(KnobOutputFile.Value().c_str());
    //out << profile.StringLong();
    out.close();
}

/* ===================================================================== */

int main(int argc, char *argv[])
{
    PIN_InitSymbols();
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    logAccesses = KnobAccessLog.Value();
    out.open(KnobOutputFile.Value().c_str());

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddSyscallEntryFunction(SyscallEntry, 0);
    PIN_AddSyscallExitFunction(SyscallExit, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns

    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
