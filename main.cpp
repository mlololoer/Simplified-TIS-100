#include "TIS-100.h"
using namespace std;
int main()
{
    cout << "Welcome to simplified TIS-100! Write your own program in main.cpp or copy paste the ones provided in the comments." << endl;
    cout << "Instruction format: [label]:[opcode] [param1] [param2]"<<endl;
    cout << "[opcode]: The instruction you want to run. It can take 0,1 or 2 parameters."<<endl;
    cout << "[label]: A jump label. Indicates a line to jump to." << endl;
    cout << "[param1] and [param2]: Parameters for your instruction. They could be a location, a number, a jump label to jump to, etc, depending on your instruction."<<endl;
	cout << "Locations: ACC: Accumulator. An all-purpose register that you can write or read from and manipulate."<<endl;
	cout << "           BAK: Secondary accumulator. You can only read and write to this register from the ACC. Use SAV to write ACC into BAK and SWP to swap them around."<<endl;
	cout << "           STK: Stack memory. Moving a value into STK will push a value. Reading from it will pop the value at the top. Follows the FILO data structure."<<endl;
	cout << "           INP and OUT: Input and output. You can only read from INP and write to OUT. Reading from INP will remove the next value from the array of inputs. Writing to OUT outputs to console."<<endl;
	/*
	sample program 1: double the input.
	string program[] = {
							"mov inp acc",
							"add acc",
							"mov acc out"
						};
	sample program 2: output a 1 if the number is even. 0 if odd. (positive inputs)
	string program[] = {
							"start:mov inp acc",
							"loop:jlz exit",
							"sub 2",
							"jmp loop",
							"exit:add 1",
							"jez odd",
							"mov 1 out",
							"jmp start",
							"odd:mov 0 out",
							"jmp start"
						};
	sample program 3: print out a set of numbers in reverse. sets are separated by zeroes.
	string program[] = {
							"start:mov inp acc",
							"jez print",
							"mov acc stk",
							"swp",
							"add 1",
							"swp",
							"jmp start",
							"print:swp",
							"loop:jez start",
							"mov stk out",
							"sub 1",
							"jmp loop"
	};
	example input: {16,5,40,0,29,29,3,66,100,0,40,7,12,0}
	output: {40,5,16,100,66,3,29,29,12,7,40}


	sample program 4: return the sum of a set of numbers. sets are separated by zeroes.
	string program[] = {
							"start:mov inp acc",
							"jez print",
							"mov acc stk",
							"swp",
							"add stk",
							"swp",
							"jmp start",
							"print:swp",
							"mov acc out",
							"mov 0 acc",
							"sav",
							"jmp start"
	};
	example input: {16,5,40,0,29,29,3,66,100,0,40,7,12,0}
	output: {61,227,59}
	*/
	short input[] = {16,5,40,0,29,29,3,66,100,0,40,7,12,0};

	string program[] = {
							"start:mov inp acc",
							"jez print",
							"mov acc stk",
							"swp",
							"add stk",
							"swp",
							"jmp start",
							"print:swp",
							"mov acc out",
							"mov 0 acc",
							"sav",
							"jmp start"
	};

	TIS100 tis100(program, sizeof(program)/sizeof(string));
	tis100.run(input, sizeof(input)/sizeof(short));
    return 0;
}
