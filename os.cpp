/*****************************************************************************
 * Mark Baltes
 * os.cpp
 * 4/26/15
 * This program runs the OS.
 *****************************************************************************/

#include <iostream>
#include "Assembler.h"
#include "VirtualMachine.h"

int main(int argc, char**argv)
{
    Assembler as;
    VirtualMachine vm;
    as.assemble(argv[1]);
    vm.run(argv[1]);
}