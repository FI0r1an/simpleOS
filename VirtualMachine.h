/*****************************************************************************
 * Mark Baltes
 * VirtualMachine.h
 * 4/11/15
 * Class description for VirtualMachine.
 
 The virtual machine initializes its memory from a .o file, enters a fetch
 and execute cycle until a halt command or error is reached, and then prints
 the result of the program to a .out file.
 *****************************************************************************/

 #ifndef VIRTUALMACHINE_H
 #define VIRTUALMACHINE_H

 #include <vector>
 #include <map>
 #include <string>

 class VirtualMachine 
 {
 private:
    typedef void (VirtualMachine::*FP)(int);

    static const int REG_FILE_SIZE = 4;
    static const int MEM_SIZE = 256;
    std::string file_name;
    std::vector<int> r;
    std::vector<int> mem;
    std::map<int, FP> instr;
    int pc, ir, sr, sp, clock, base, limit;

    // each instruction has its own function
    void load(int x);
    void store(int x);
    void add(int x);
    void addc(int x);
    void sub(int x);
    void subc(int x);
    void and_(int x);
    void xor_(int x);
    void compl_(int x);
    void shl(int x);
    // void shla(int x); // Not clear on what the functions shla or shra do.
    void shr(int x);     // Witheld for the moment.
    // void shra(int x);
    void compr(int x);
    void getstat(int x);
    void putstat(int x);
    void jump(int x);
    void jumpl(int x);
    void jumpe(int x);
    void jumpg(int x);
    void call(int x);
    void return_(int x);
    void read(int x);
    void write(int x);
    void halt(int x);
    void noop(int x);
public:
    VirtualMachine();
    void run(char*);
    bool replace(std::string&, const std::string&, const std::string&);
 };

 #endif