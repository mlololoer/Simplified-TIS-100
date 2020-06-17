#ifndef TIS_100_H_INCLUDED
#define TIS_100_H_INCLUDED
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <string>
#include <windows.h>
#undef OUT


using namespace std;

enum Color {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY, DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE};

void consoleMove(const int& x, const int& y);
void setColor(Color fg, Color bg);

enum Location {NOLOC,ACC,INP,OUT,STK};
enum Opcode {UNKNOWN,NOP,SWP,SAV,NEG,ADD,SUB,MOV,JMP,JEZ,JNZ,JGZ,JLZ,JRO};
struct Instruction {
	Opcode opcode;
	Location source;
	Location destination;
	short label;
};
const int maxInstructions = 15;
const int maxStkSize = 15;

class TIS100 {
	const int numInstructions;
	short stkSize = 0;
	short PC = 0;
	short acc = 0;
	short bak = 0;

	short stk[maxStkSize];
	bool push(short input);
	short pop();

	Instruction program[maxInstructions];
	bool errorsExist = false;
	string labels[maxInstructions];
	Opcode convertOpcode(string input);
	Location convertLocation(string input);
	void printInstruction(Instruction input);
public:
	TIS100(const string input[], int numInstructions);
	void run(short input[], const int inputSize);
};

#endif // TIS-100_H_INCLUDED
