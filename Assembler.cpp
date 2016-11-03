/*****************************************************************************
 * Mark Baltes
 * Assembler.cpp
 *
 * Implementation file for Assembler class.
 *****************************************************************************/

 #include "Assembler.h"
 #include <cstdlib>
 #include <fstream>
 #include <sstream>
 #include <stdexcept>

Assembler::Assembler() 
{
    instr["load"] = &Assembler::load;
    instr["loadi"] = &Assembler::loadi;
    instr["store"] = &Assembler::store;
    instr["add"] = &Assembler::add;
    instr["addi"] = &Assembler::addi;
    instr["addc"] = &Assembler::addc;
    instr["addci"] = &Assembler::addci;
    instr["sub"] = &Assembler::sub;
    instr["subi"] = &Assembler::subi;
    instr["subc"] = &Assembler::subc;
    instr["subci"] = &Assembler::subci;
    instr["and"] = &Assembler::and_;
    instr["andi"] = &Assembler::andi;
    instr["xor"] = &Assembler::xor_;
    instr["xori"] = &Assembler::xori;
    instr["compl"] = &Assembler::compl_;
    instr["shl"] = &Assembler::shl;
    instr["shla"] = &Assembler::shla;
    instr["shr"] = &Assembler::shr;
    instr["shra"] = &Assembler::shra;
    instr["compr"] = &Assembler::compr;
    instr["compri"] = &Assembler::compri;
    instr["getstat"] = &Assembler::getstat;
    instr["putstat"] = &Assembler::putstat;
    instr["jump"] = &Assembler::jump;
    instr["jumpl"] = &Assembler::jumpl;
    instr["jumpe"] = &Assembler::jumpe;
    instr["jumpg"] = &Assembler::jumpg;
    instr["call"] = &Assembler::call;
    instr["return"] = &Assembler::return_;
    instr["read"] = &Assembler::read;
    instr["write"] = &Assembler::write;
    instr["halt"] = &Assembler::halt;
    instr["noop"] = &Assembler::noop;
}

void Assembler::assemble(char* file)
{
    // Create program files
    this->file_name = file;

    // Create and clear .o file
    Assembler::replace(file_name, ".s", ".o");
    std::ofstream outfile(file_name.c_str());
    outfile.open(file_name.c_str(), std::ios::out | std::ios::trunc);
    outfile.close();

    // Create .out file
    Assembler::replace(file_name, ".o", ".out");
    std::ofstream outfile_out(file_name.c_str());
    outfile_out.close();

    // Create .in file // taking out creating .in file for now
    // Assembler::replace(file_name, ".out", ".in");
    // std::ofstream outfile_in(file_name.c_str());
    // outfile_in.close();
    this->file_name = file; // resetting file so that print function can use it.

    std::fstream assemble;
    std::string opcode, line;

    assemble.open(this->file_name.c_str(), std::ios::in);
    if (!assemble.is_open()) {
        std::cout << this->file_name << " failed to open.\n";
        exit(1);
    }

    getline(assemble, line);
    while (!assemble.eof()) {
        std::istringstream str(line.c_str());

        str >> opcode;

        if (opcode == "!") { // Line is comment, so skip line.
            getline(assemble, line);
            continue;
        }

        if (not instr[opcode]) {
            std::cout << "Error: assemble" << "\n";
        } else {
            (this->*instr[opcode])(line);
        }

        getline(assemble, line);
    }

    assemble.close();
}

void Assembler::load(std::string s)
{
    std::string opcode;
    int rd = 0, addr = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> addr;

    if ((rd >= 0 and rd < 4) and (addr >= 0 and addr < 256)) {
        //int immediate = 0;
        // set rd
        instruction += (rd << 9);
        // clear immediate bit
        instruction &= ~(1 << 8);
        // set addr
        instruction += (addr << 0);
    } else {
        std::cout << "Error in load: runtime error." << "\n";
        exit(1);
    }

    print(instruction);
}

void Assembler::loadi(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in loadi: invalid entry." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::store(std::string s)
{
    std::string opcode;
    int rd = 0, addr = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> addr;

    if ((rd >= 0 and rd < 4) and (addr >= 0 and addr < 256)) {
        int immediate = 1;
        // move instruction into place
        instruction += (1 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set addr bits
        instruction += (addr << 0);
    } else {
        std::cout << "Error in store: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::add(std::string s)
{
    std::string opcode;
    int rd = 0, rs = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> rs;

    if ((rd >= 0 and rd < 4) and (rs >= 0 and rs < 4)) {
        int immediate = 0; // do i need?
        // set instruction
        instruction += (2 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (rs << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in add: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::addi(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set opcode
        instruction += (2 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant.
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in addi: " << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::addc(std::string s)
{
    std::string opcode;
    int rd = 0, rs = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> rs;

    if ((rd >= 0 and rd < 4) and (rs >= 0 and rs < 4)) {
        int immediate = 0; // do i need?
        // set instruction
        instruction += (3 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (rs << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in addc: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::addci(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set opcode
        instruction += (3 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant. 
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in addci: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::sub(std::string s)
{
    std::string opcode;
    int rd = 0, rs = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> rs;

    if ((rd >= 0 and rd < 4) and (rs >= 0 and rs < 4)) {
        int immediate = 0; // do i need?
        // set instruction
        instruction += (4 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (rs << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in sub: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::subi(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set opcode
        instruction += (4 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant. 
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in subi: " << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::subc(std::string s)
{
    std::string opcode;
    int rd = 0, rs = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> rs;

    if ((rd >= 0 and rd < 4) and (rs >= 0 and rs < 4)) {
        int immediate = 0; // do i need?
        // set instruction
        instruction += (5 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (rs << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in subc: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::subci(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set opcode
        instruction += (5 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant. 
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in subci: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::and_(std::string s)
{
    std::string opcode;
    int rd = 0, rs = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> rs;

    if ((rd >= 0 and rd < 4) and (rs >= 0 and rs < 4)) {
        int immediate = 0; // do i need?
        // set instruction
        instruction += (6 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (rs << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in and: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::andi(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set opcode
        instruction += (6 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant. 
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in andi: " << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::xor_(std::string s)
{
    std::string opcode;
    int rd = 0, rs = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> rs;

    if ((rd >= 0 and rd < 4) and (rs >= 0 and rs < 4)) {
        int immediate = 0; // do i need?
        // set instruction
        instruction += (7 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (rs << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in xor: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::xori(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set opcode
        instruction += (7 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in xori: " << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::compl_(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (8 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in compl: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::shl(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (9 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in shl: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::shla(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (10 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in shla: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::shr(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (11 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in shr: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::shra(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (12 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in shra: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::compr(std::string s)
{
    std::string opcode;
    int rd = 0, rs = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> rs;

    if ((rd >= 0 and rd < 4) and (rs >= 0 and rs < 4)) {
        int immediate = 0; // do i need?
        // set instruction
        instruction += (13 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (rs << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in compr: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::compri(std::string s)
{
    std::string opcode;
    int rd = 0, constant = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd >> constant;

    if ((rd >= 0 and rd < 4) and (constant >= -128 and constant < 128)) {
        int immediate = 1;
        // set opcode
        instruction += (13 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction |= immediate << 8;
        // add addr/constant
        constant &= 255; // getting rid of all preceding 0's
        instruction += (constant << 0);
    } else {
        std::cout << "Error in compri: " << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::getstat(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (14 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in getstat: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::putstat(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (15 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in putstat: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::jump(std::string s)
{
    std::string opcode;
    int addr = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> addr;

    if (addr >= 0 and addr < 256) {
        int immediate = 1; 
        // set instruction
        instruction += (16 << 11);
        // set rd bits
        instruction += (0 << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // set addr bits
        instruction += (addr << 0);
    } else {
        std::cout << "Error in jump: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::jumpl(std::string s)
{
    std::string opcode;
    int addr = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> addr;

    if (addr >= 0 and addr < 256) {
        int immediate = 1; 
        // set instruction
        instruction += (17 << 11);
        // set rd bits
        instruction += (0 << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // set addr bits
        instruction += (addr << 0);
    } else {
        std::cout << "Error in jumpl: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::jumpe(std::string s)
{
    std::string opcode;
    int addr = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> addr;

    if (addr >= 0 and addr < 256) {
        int immediate = 1; 
        // set instruction
        instruction += (18 << 11);
        // set rd bits
        instruction += (0 << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // set addr bits
        instruction += (addr << 0);
    } else {
        std::cout << "Error in jumpe: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::jumpg(std::string s)
{
    std::string opcode;
    int addr = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> addr;

    if (addr >= 0 and addr < 256) {
        int immediate = 1; 
        // set instruction
        instruction += (19 << 11);
        // set rd bits
        instruction += (0 << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // set addr bits
        instruction += (addr << 0);
    } else {
        std::cout << "Error in jumpg: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::print(int n)
{
    std::fstream output_file;
    std::string file_o(this->file_name);
    Assembler::replace(file_o, ".s", ".o");

    output_file.open(file_o.c_str(), std::ios::out | std::ios::app);
    if (!output_file.is_open()) {
        std::cout << file_o << " failed to open.\n";
        exit(1);
    }

    output_file << n << "\n";

    output_file.close();
}

void Assembler::read(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (22 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in read: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::write(std::string s)
{
    std::string opcode;
    int rd = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> rd;

    if (rd >= 0 and rd < 4) {
        int immediate = 0; 
        // set instruction
        instruction += (23 << 11);
        // set rd bits
        instruction += (rd << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Error in write: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::call(std::string s)
{
    std::string opcode;
    int addr = 0, instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode >> addr;

    if (addr >= 0 and addr < 256) {
        int immediate = 1; 
        // set instruction
        instruction += (20 << 11);
        // set rd bits
        instruction += (0 << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // set addr bits
        instruction += (addr << 0);
    } else {
        std::cout << "Error in call: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::return_(std::string s)
{
    std::string opcode;
    //int rd = 0; 
    int instruction = 0;

    std::istringstream str(s.c_str());
    str >> opcode;

    if (1 == 1) { // we always want to run this (could remove if)
        int immediate = 0; 
        // set instruction
        instruction += (21 << 11);
        // set rd bits
        instruction += (0 << 9);
        // set immediate bit
        instruction += (immediate << 8);
        // set rs bits
        instruction += (0 << 6);
        // clear unused
        instruction += (0 << 0);
    } else {
        std::cout << "Errorin return: Invalid input." << "\n";
        exit(1);
    }

    print(instruction); // outputs line to prog.o
}

void Assembler::halt(std::string s)
{
    int instruction = 0;

    // set instruction
    instruction += (24 << 11);
    print(instruction); // outputs line to prog.o
}

void Assembler::noop(std::string s)
{
    int instruction = 0;
    // set instruction
    instruction += (25 << 11);
    print(instruction); // outputs line to prog.o
}

bool Assembler::replace(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos) {
        return false;
    }
    str.replace(start_pos, from.length(), to);
    return true;
}

// void Assembler::make_files(std::string s)
// {
//     // Create .o file
//     replace(s, ".s", ".o");
//     std::ofstream outfile(s.c_str());
//     outfile.close();

//     // Create .out file
//     replace(s, ".o", ".out");
//     std::ofstream outfile_out(s.c_str());
//     outfile_out.close();

//     // Create .in file
//     replace(s, ".out", ".in");
//     std::ofstream outfile_in(s.c_str());
//     outfile_in.close();
// }