/*****************************************************************************
 * Mark Baltes
 * VirtualMachine.cpp
 * 
 * Implementation file for VirtualMachine class.
 *****************************************************************************/

#include "VirtualMachine.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

VirtualMachine::VirtualMachine() // constructor
{
    //r = std::vector<int>(REG_FILE_SIZE);
    //mem = std::vector<int>(MEM_SIZE);
    r.reserve(REG_FILE_SIZE);
    mem.reserve(MEM_SIZE);

    instr[0] = &VirtualMachine::load; // loadi inside
    instr[1] = &VirtualMachine::store;
    instr[2] = &VirtualMachine::add; // addi is inside
    instr[3] = &VirtualMachine::addc; // addci is inside
    instr[4] = &VirtualMachine::sub; // subi is inside
    instr[5] = &VirtualMachine::subc; // subci inside
    instr[6] = &VirtualMachine::and_; // andi inside
    instr[7] = &VirtualMachine::xor_; // xori inside
    instr[8] = &VirtualMachine::compl_;
    instr[9] = &VirtualMachine::shl;
    // instr[10] = &VirtualMachine::shla; // Withheld.
    instr[11] = &VirtualMachine::shr;
    // instr[12] = &VirtualMachine::shra; // Withheld.
    instr[13] = &VirtualMachine::compr; // compri inside
    instr[14] = &VirtualMachine::getstat;
    instr[15] = &VirtualMachine::putstat;
    instr[16] = &VirtualMachine::jump;
    instr[17] = &VirtualMachine::jumpl;
    instr[18] = &VirtualMachine::jumpe;
    instr[19] = &VirtualMachine::jumpg;
    instr[20] = &VirtualMachine::call;
    instr[21] = &VirtualMachine::return_;
    instr[22] = &VirtualMachine::read;
    instr[23] = &VirtualMachine::write;
    instr[24] = &VirtualMachine::halt;
    instr[25] = &VirtualMachine::noop;
}

void VirtualMachine::run(char* file)
{
    // Transform file argument to .o
    this->file_name = file;
    VirtualMachine::replace(file_name, ".s", ".o");
    // End transform
    
    std::fstream populate;

    populate.open(this->file_name.c_str(), std::ios::in);
    if (!populate.is_open()) {
        std::cout << file_name << " failed to open.\n";
        exit(1);
    }

    int current_value = 0, counter = 0;
    populate >> current_value;
    while (!populate.eof()) {
        mem.push_back(current_value);
        ++counter;
        populate >> current_value;
    }

    populate.close(); 
    // restore file_name
    this->file_name = file;

    // Initialize VM 
    int opcode = 0, ir_ = 0;
    limit = counter;
    pc = 0; 
    base = 0; 
    clock = 0; 
    sp = 256; 
    sr = 0;

    // Enter instruction fetch-execute infinite loop
    opcode = (mem[pc] >> 11);
    while (opcode != 24) {
        ir_ = mem[pc];
        ++pc;
        (this->*instr[opcode])(ir_);
        // debugging information
        std::cout << "r0: " << r[0] << "\n";
        std::cout << "r1: " << r[1] << "\n";
        std::cout << "r2: " << r[2] << "\n";
        std::cout << "r3: " << r[3] << "\n";
        std::cout << "pc: " << pc << "\n";
        // std::cout.flush();
        // usleep(1000000);
        // end debug info
        opcode = (mem[pc] >> 11);
    }

    std::cout << "Halt encountered." << "\n";
    std::cout << "Clock: " << clock << "\n";
}

void VirtualMachine::read(int ir)
{
    // Transform file argument to .o
    std::string temp_file_name = this->file_name;
    VirtualMachine::replace(this->file_name, ".s", ".in");

    std::fstream infile;
    int file_num = 0, dest = 0;
    infile.open(this->file_name.c_str(), std::ios::in);
    if (!infile.is_open()) {
        std::cout << this->file_name << " failed to open.\n";
        exit(1);
    }

    infile >> file_num;
    infile.close();
    // restore file_name
    this->file_name = temp_file_name;

    dest += (ir >> 9);
    dest &= 3;
    r[dest] = file_num;

    clock += 28;
    std::cout <<"Read fn. Clock = " << clock << "\n";
}

void VirtualMachine::load(int ir)
{
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // load
        // getting addr value
        int addr = 0, rd = 0;
        addr += (ir >> 0);
        addr &= 255;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // set values
        r[rd] = mem[addr];
        // set clock
        clock += 4;
        std::cout <<"Load fn im == 0. Clock = " << clock << "\n";
    } else if (imm == 1) { // loadi
        // getting constant value
        int constant = 0, rd = 0;
        constant += (ir >> 0);
        constant &= 255; // from 63
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // set values
        r[rd] = constant;
        // set clock
        clock += 1;
        std::cout <<"Loadi fn. Clock = " << clock << "\n";
    } else { // immediate value incorrect
        std::cout << "Error: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::store(int ir)
{
    int rd = 0, addr = 0;
    // getting rd value
    rd += (ir >> 9);
    rd &= 3;
    // getting addr value
    addr += (ir >> 0);
    addr &= 255;
    // set values
    mem[addr] = r[rd];
    // set clock
    clock += 4;
    std::cout <<"Store fn. Clock = " << clock << "\n";
}

void VirtualMachine::add(int ir)
{
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // add
        int rd = 0, rs = 0;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting rs value
        rs += (ir >> 6);
        rs &= 3;
        // set values
        r[rd] = r[rd] + r[rs];
        // set clock
        clock += 1;
        std::cout <<"Add fn. Clock = " << clock << "\n";
    } else if (imm == 1) { // addi
        int rd = 0, constant = 0;
        // getting constant value
        constant += (ir >> 0);
        constant &= 255;
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // set values
        r[rd] = r[rd] + constant;
        // set clock
        clock += 1;
        std::cout <<"Addi fn. Clock = " << clock << "\n";
    } else { // immediate value invalid
        std::cout << "Error: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::addc(int ir)
{
    // get immediate bit
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // addc
        int rd = 0, rs = 0;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting rs value
        rs += (ir >> 6);
        rs &= 3;
        // getting carry value
        int carry = 0;
        carry += (sr >> 0);
        carry &= 1;
        // set values
        r[rd] = r[rd] + r[rs] + carry;
        // set clock
        clock += 1;
        std::cout <<"Addc fn. Clock = " << clock << "\n";
    } else if (imm == 1) { // addci
        int rd = 0, constant = 0;
        // getting constant value
        constant += (ir >> 0);
        constant &= 255;
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting carry value
        int carry = 0;
        carry += (sr >> 0);
        carry &= 1;
        // set values
        r[rd] = r[rd] + constant + carry;
        // set clock
        clock += 1;
        std::cout <<"Addci fn. Clock = " << clock << "\n";
    } else { // immediate value invalid
        std::cout << "Error in addc: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::sub(int ir)
{
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // sub
        int rd = 0, rs = 0;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting rs value
        rs += (ir >> 6);
        rs &= 3;
        // set values
        r[rd] = r[rd] - r[rs];
        // set clock
        clock += 1;
        std::cout <<"Add fn. Clock = " << clock << "\n";
    } else if (imm == 1) { // subi
        int rd = 0, constant = 0;
        // getting constant value
        constant += (ir >> 0);
        constant &= 255;
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // set values
        r[rd] = r[rd] - constant;
        // set clock
        clock += 1;
        std::cout <<"Addi fn. Clock = " << clock << "\n";
    } else { // immediate value invalid
        std::cout << "Error: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::subc(int ir)
{
    // get immediate bit
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // subc
        int rd = 0, rs = 0;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting rs value
        rs += (ir >> 6);
        rs &= 3;
        // getting carry value
        int carry = 0;
        carry += (sr >> 0);
        carry &= 1;
        // set values
        r[rd] = r[rd] - r[rs] - carry;
        // set clock
        clock += 1;
        std::cout <<"Subc fn. Clock = " << clock << "\n";
    } else if (imm == 1) { // subci
        int rd = 0, constant = 0;
        // getting constant value
        constant += (ir >> 0);
        constant &= 255;
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting carry value
        int carry = 0;
        carry += (sr >> 0);
        carry &= 1;
        // set values
        r[rd] = r[rd] - constant - carry;
        // set clock
        clock += 1;
        std::cout <<"Subci fn. Clock = " << clock << "\n";
    } else { // immediate value invalid
        std::cout << "Error in addc: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::and_(int ir)
{
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // and_
        int rd = 0, rs = 0;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting rs value
        rs += (ir >> 6);
        rs &= 3;
        // set values
        r[rd] = r[rd] & r[rs];
        // set clock
        clock += 1;
        std::cout <<"And_ fn. Clock = " << clock << "\n";
    } else if (imm == 1) { // andi
        int rd = 0, constant = 0;
        // getting constant value
        constant += (ir >> 0);
        constant &= 255;
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // set values
        r[rd] = r[rd] & constant;
        // set clock
        clock += 1;
        std::cout <<"Andi fn. Clock = " << clock << "\n";
    } else { // immediate value invalid
        std::cout << "Error: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::xor_(int ir)
{
    // check immediate bit
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // xor_
        int rd = 0, rs = 0;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting rs value
        rs += (ir >> 6);
        rs &= 3;
        // set values
        r[rd] = r[rd] ^ r[rs];
        // set clock
        clock += 1;
        std::cout <<"Xor_ fn. Clock = " << clock << "\n";
    } else if (imm == 1) { // xori_
        int rd = 0, constant = 0;
        // getting constant value
        constant += (ir >> 0);
        constant &= 255;
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // set values
        r[rd] = r[rd] ^ constant;
        // set clock
        clock += 1;
        std::cout <<"Xori_ fn. Clock = " << clock << "\n";
    } else { // immediate value invalid
        std::cout << "Error: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::compl_(int ir)
{
    // get rd value
    int rd = 0;
    rd += (ir >> 9);
    rd &= 3;
    // action
    r[rd] = ~r[rd];
    // clock
    clock += 1;
    std::cout << "Compl_ fn. Clock = " << clock << "\n";
}

void VirtualMachine::shl(int ir)
{
    // get rd value
    int rd = 0;
    rd += (ir >> 9);
    rd &= 3;
    // carry
    // int temp = 0;
    // temp += (r[rd] >> 7);
    // if (temp == 1) { // need to carry?
    //     sr |= 1 << 0;
    // }
    // shift bits
    r[rd] = r[rd] << 1;

    clock += 1;
    std::cout <<"Shl fn. Clock = " << clock << "\n";
}

void VirtualMachine::shr(int ir)
{
    // get rd value
    int rd = 0;
    rd += (ir >> 9);
    rd &= 3;
    // shift bits
    r[rd] = r[rd] >> 1;

    clock += 1;
    std::cout <<"Shr fn. Clock = " << clock << "\n";
}

void VirtualMachine::compr(int ir)
{
    int imm = 0;
    imm += (ir >> 8);
    imm &= 1;

    if (imm == 0) { // compr
        int rd = 0, rs = 0;
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // getting rs value
        rs += (ir >> 6);
        rs &= 3;
        // set values
        if (r[rd] < r[rs]) { // set less and reset equal and greater.
            sr |= 1 << 3;
            sr &= ~(1 << 2);
            sr &= ~(1 << 1);
        } else if (r[rd] == r[rs]) { // set equal, reset less and greater.
            sr |= 1 << 2;
            sr &= ~(1 << 3);
            sr &= ~(1 << 1);
        } else if (r[rd] > r[rs]) { // set greater, reset equal and less.
            sr |= 1 << 1;
            sr &= ~(1 << 2);
            sr &= ~(1 << 3);
        } else {
            std::cout <<"Error in compr: invalid comparison." << "\n";
            exit(1);
        }
        // set clock
        clock += 1;
        std::cout <<"Add fn. Clock = " << clock << "\n";
    } else if (imm == 1) { // compri
        int rd = 0, constant = 0;
        // getting constant value
        constant += (ir >> 0);
        constant &= 255;
        // check if negative
        int temp = constant >> 7;
        temp &= 1;
        if (temp == 1) { // neg number
            constant += 0xFFFFFF00;
        }
        // getting rd value
        rd += (ir >> 9);
        rd &= 3;
        // set values
        if (r[rd] < constant) { // set less and reset equal and greater.
            sr |= 1 << 3;
            sr &= ~(1 << 2);
            sr &= ~(1 << 1);
        } else if (r[rd] == constant) { // set equal, reset less and greater.
            sr |= 1 << 2;
            sr &= ~(1 << 3);
            sr &= ~(1 << 1);
        } else if (r[rd] > constant) { // set greater, reset equal and less.
            sr |= (1 << 1);
            sr &= ~(1 << 2);
            sr &= ~(1 << 3);
            std::cout << "from compri, sr = " << sr << "\n";
        } else {
            std::cout <<"Error in compri: invalid comparison." << "\n";
            exit(1);
        }
        // set clock
        clock += 1;
        std::cout <<"Compri fn. Clock = " << clock << "\n";
    } else { // immediate value invalid
        std::cout << "Error: Invalid immediate value." << "\n";
        exit(1);
    }
}

void VirtualMachine::getstat(int ir)
{
    // get rd value
    int rd = 0;
    rd += (ir >> 9);
    rd &= 3;
    // get carry
    int carry = 0;
    carry += (sr >> 0);
    carry &= 1;
    // action
    // r[rd] = carry;
    r[rd] = sr;
    // clock
    clock += 1;
    std::cout << "Getstat fn. Clock = " << clock << "\n";
}

void VirtualMachine::putstat(int ir)
{
    // get rd value
    int rd = 0;
    rd += (ir >> 9);
    rd &= 3;
    // get carry
    int carry = 0;
    carry += (sr >> 0);
    carry &= 1;
    // action
    sr = r[rd];
    // carry = r[rd];
    // sr += (carry << 0);
    // clock
    clock += 1;
    std::cout << "Putstat fn. Clock = " << clock << "\n";
}

void VirtualMachine::jump(int ir)
{
    // getting addr value
    int addr = 0;
    addr += (ir >> 0);
    addr &= 255;
    // setting pc value
    pc = addr;
    // clock
    clock += 1;
    std::cout <<"Jump fn. Clock = " << clock << "\n";
}

void VirtualMachine::jumpl(int ir)
{
    int less = 0;
    less += (sr >> 3);
    less &= 1;

    if (less == 1) {
        // getting addr value
        int addr = 0;
        addr += (ir >> 0);
        addr &= 255;
        // setting pc value
        pc = addr;
    }
    // clock
    clock += 1;
    std::cout <<"Jumpl fn. Clock = " << clock << "\n";
}

void VirtualMachine::jumpe(int ir)
{
    int equal = 0;
    equal += (sr >> 2);
    equal &= 1;

    if (equal == 1) {
        // getting addr value
        int addr = 0;
        addr += (ir >> 0);
        addr &= 255;
        // setting pc value
        pc = addr;
    }
    // clock
    clock += 1;
    std::cout <<"Jumpe fn. Clock = " << clock << "\n";
}

void VirtualMachine::jumpg(int ir)
{
    int greater = 0;
    greater += (sr >> 1);
    greater &= 1;

    if (greater == 1) {
        // getting addr value
        int addr = 0;
        addr += (ir >> 0);
        addr &= 255;
        // setting pc value
        pc = addr;
    }
    //clock
    clock += 1;
    std::cout <<"Jumpg fn. Clock = " << clock << "\n";
}

void VirtualMachine::write(int ir)
{
    // Transform file argument to .o
    std::string temp_file_name = this->file_name;
    VirtualMachine::replace(this->file_name, ".s", ".out");

    std::fstream output_file;

    output_file.open(this->file_name.c_str(), std::ios::out | std::ios::trunc);
    if (!output_file.is_open()) {
        std::cout << this->file_name << " failed to open.\n";
        exit(1);
    }

    // get rd value
    int rd = 0;
    rd += (ir >> 9);
    rd &= 3;

    output_file << r[rd] << "\n";
    clock += 28;
    output_file << "Clock: " << clock << "\n";

    output_file.close();
    // restore file_name
    this->file_name = temp_file_name;

    
    std::cout <<"Write fn. Clock = " << clock << "\n";
}

void VirtualMachine::call(int ir)
{
    // Save state of VM
    mem[--sp] = pc;
    mem[--sp] = r[0];
    mem[--sp] = r[1];
    mem[--sp] = r[2];
    mem[--sp] = r[3];
    mem[--sp] = sr;
    // Update pc
    int addr = 0;
    addr += (ir >> 0);
    addr &= 255;
    pc = addr;
    // clock
    clock += 4;
    std::cout <<"Call fn. Clock = " << clock << "\n";

}

void VirtualMachine::return_(int ir)
{
    // Restore VM state to registers
    sr = mem[sp];
    mem[sp] = 0; // pop
    r[3] = mem[++sp];
    mem[sp] = 0;
    r[2] = mem[++sp];
    mem[sp] = 0;
    r[1] = mem[++sp];
    mem[sp] = 0;
    r[0] = mem[++sp];
    mem[sp] = 0;
    pc = mem[++sp];
    ++sp;
    // clock
    clock += 4;
}

void VirtualMachine::halt(int ir)
{
    std::cout << "VM halting." << "\n";
}

void VirtualMachine::noop(int ir) {
    // does nothing
}

bool VirtualMachine::replace(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos) {
        return false;
    }
    str.replace(start_pos, from.length(), to);
    return true;
}