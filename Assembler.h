/*****************************************************************************
 * Mark Baltes
 * Assembler.h
 * 
 * Header file for Assembler class.

 The assembler parses a .s file containing commands for the OS, and outputs
 a .o file containing the corresponding operation integers in a format the
 virtual machine understands.
 *****************************************************************************/

 #ifndef ASSEMBLER_H
 #define ASSEMBLER_H

 #include <iostream>
 #include <map>
 #include <string>

 class Assembler {
    // Define Function Pointer type.
    typedef void (Assembler::*FP)(std::string);

    // Each instruction has its own function.
    void load(std::string s);
    void loadi(std::string s);
    void store(std::string s);
    void add(std::string s);
    void addi(std::string s);
    void addc(std::string s);
    void addci(std::string s);
    void sub(std::string s);
    void subi(std::string s);
    void subc(std::string s);
    void subci(std::string s);
    void and_(std::string s);
    void andi(std::string s);
    void xor_(std::string s);
    void xori(std::string s);
    void compl_(std::string s);
    void shl(std::string s);
    void shla(std::string s);
    void shr(std::string s);
    void shra(std::string s);
    void compr(std::string s);
    void compri(std::string s);
    void getstat(std::string s);
    void putstat(std::string s);
    void jump(std::string s);
    void jumpl(std::string s);
    void jumpe(std::string s);
    void jumpg(std::string s);
    void call(std::string s);
    void return_(std::string s);
    void read(std::string s);
    void write(std::string s);
    void halt(std::string s);
    void noop(std::string s);
    void print(int n);
    
    // The map.
    std::map<std::string, FP> instr;
    std::string file_name;
public:
    Assembler();
    void assemble(char*);
    //void make_files();
    bool replace(std::string&, const std::string&, const std::string&);
};

 #endif