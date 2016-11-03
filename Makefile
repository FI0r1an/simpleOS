all: os

os: Assembler.o VirtualMachine.o os.o
	g++ -g -Wall -o os os.o Assembler.o VirtualMachine.o

Assembler.o: Assembler.cpp
	g++ -g -Wall -c Assembler.cpp

VirtualMachine.o: VirtualMachine.cpp
	g++ -g -Wall -c VirtualMachine.cpp

os.o: os.cpp
	g++ -g -Wall -c os.cpp
