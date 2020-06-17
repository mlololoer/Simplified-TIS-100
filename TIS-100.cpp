#include "TIS-100.h"
using std::cout;
using std::cin;
using std::endl;
using std::string;

void consoleMove(const int& x, const int& y) {
	COORD a = {static_cast<short>(x),static_cast<short>(y)};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),a);
}

void setColor(Color fg = LIGHTGRAY, Color bg = BLACK) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg + (bg<<4));
}

Opcode TIS100::convertOpcode(string input) {
	if (input == "nop") return NOP;
	else if (input == "mov") return MOV;
	else if (input == "swp") return SWP;
	else if (input == "sav") return SAV;
	else if (input == "add") return ADD;
	else if (input == "sub") return SUB;
	else if (input == "neg") return NEG;
	else if (input == "jmp") return JMP;
	else if (input == "jez") return JEZ;
	else if (input == "jnz") return JNZ;
	else if (input == "jgz") return JGZ;
	else if (input == "jlz") return JLZ;
	else if (input == "jro") return JRO;
	else  return UNKNOWN;
}

Location TIS100::convertLocation(string input) {
	if (input == "acc") return ACC;
	else if (input == "inp") return INP;
	else if (input == "out") return OUT;
	else if (input == "stk") return STK;
	else return NOLOC;
}

void TIS100::printInstruction(Instruction input) {
	switch (input.opcode) {
		case UNKNOWN:cout << "UNKNOWN";break;
		case NOP:cout << "NOP";break;
		case SWP:cout << "SWP";break;
		case SAV:cout << "SAV";break;
		case NEG:cout << "NEG";break;
		case ADD:cout << "ADD";break;
		case SUB:cout << "SUB";break;
		case MOV:cout << "MOV";break;
		case JMP:cout << "JMP";break;
		case JEZ:cout << "JEZ";break;
		case JNZ:cout << "JNZ";break;
		case JGZ:cout << "JGZ";break;
		case JLZ:cout << "JLZ";break;
		case JRO:cout << "JRO";
	}
	if (input.opcode >= JMP && input.opcode <= JLZ) {
		cout << ' ' << labels[input.label];
		return;
	} else if ((input.opcode >= ADD && input.opcode <= MOV) || input.opcode == JRO) {
		cout << ' ';
		switch (input.source) {
			case 0:cout << "NOLOC";break;
			case 1:cout << "ACC";break;
			case 2:cout << "INP";break;
			case 3:cout << input.label;break;
			case 4:cout << "STK";
		}
		if (input.opcode == MOV) {
			cout<< ' ';
			switch (input.destination) {
				case 0: cout << "NOLOC";break;
				case 1:cout << "ACC";break;
				case 2:cout << "INP";break;
				case 3:cout << "OUT";break;
				case 4:cout << "STK";
			}
		}
	}
}

TIS100::TIS100(const string input[], int num): numInstructions(num) {
	if (numInstructions > maxInstructions) cout << "Too many instructions! The program will be truncated up to line " << maxInstructions << '.' << endl;
	for (short i = 0; i < numInstructions; ++i) { //finding jump label locations
		short delimiter = 0;
		while (input[i][delimiter] != '\0' && input[i][delimiter] != ' ' && input[i][delimiter] != ':') ++delimiter;
		if (input[i][delimiter] == ':') {
			labels[i] = input[i].substr(0,delimiter);
		}
	}
	for (short i = 0; i < numInstructions; ++i) {
		short front = 0;
		short delimiter = 0;
		while (input[i][delimiter] != '\0' && input[i][delimiter] != ' ' && input[i][delimiter] != ':') ++delimiter;
		if (front == delimiter) {program[i] = {NOP,NOLOC,NOLOC,-1}; continue;}
		else if (input[i][delimiter] == ':') { //skip over labels
			front = ++delimiter;
			while (input[i][delimiter] != '\0' && input[i][delimiter] != ' ') ++delimiter;
		}
		Opcode opcode = convertOpcode(input[i].substr(front,delimiter-front));
		switch (input[i][delimiter]) {
			case '\0': {
				if (opcode < NOP || opcode > NEG) {
					errorsExist = true;
					cout << "Unknown no-parameter instruction encountered on line " << i << endl;
					program[i] = {UNKNOWN,NOLOC,NOLOC,-1};
				} else program[i] = {opcode,NOLOC,NOLOC,-1};
				break;
			}
			case ' ': {
				front = ++delimiter;
				while (input[i][delimiter] != '\0' && input[i][delimiter] != ' ') ++delimiter;
				string param1 = input[i].substr(front,delimiter-front);
				if (opcode >= JMP && opcode <= JLZ) {
					short labelIndex = -1;
					for (int j = 0; j < numInstructions; ++j) {
						if (param1 == labels[j]) labelIndex = j;
					}
					if (labelIndex == -1) {
						errorsExist = true;
						cout << "Undeclared label encountered on line " << i << endl;
					}
					program[i] = {opcode,NOLOC,NOLOC,labelIndex};
				} else if (opcode == ADD || opcode == SUB) {
					bool isOnlyNumbers = true;
					for (int j = 0; j < delimiter-front; ++j) {
						if (!((param1[j] >= '0' && param1[j] <= '9') || param1[j] == '-')) isOnlyNumbers = false;
					}
					if (isOnlyNumbers) {
						if (stoi(param1) >= -999 && stoi(param1) <= 999) {
							program[i] = {opcode,OUT,NOLOC,static_cast<short>(stoi(param1))};
						} else {
							errorsExist = true;
							cout << "Unknown immediate encountered on line " << i << endl;
							program[i] = {opcode, OUT,NOLOC,0};
						}
					} else {
						Location src = convertLocation(param1);
						if (src == NOLOC || src == OUT) {
							errorsExist = true;
							cout << "Unknown location encountered on line " << i << endl;
							program[i] = {opcode,NOLOC,NOLOC,-1};
						} else {
							program[i] = {opcode,src,NOLOC,-1};
						}
					}
				} else if (opcode == MOV) {
					bool isOnlyNumbers = true;
					for (int j = 0; j < delimiter-front; ++j) {
						if (!((param1[j] >= '0' && param1[j] <= '9') || param1[j] == '-')) isOnlyNumbers = false;
					}
					if (isOnlyNumbers) {
						if (stoi(param1) >= -999 && stoi(param1) <= 999) {
							program[i] = {opcode,OUT,NOLOC,static_cast<short>(stoi(param1))};
						} else {
							errorsExist = true;
							cout << "Unknown MOV immediate encountered on line " << i << endl;
							program[i] = {opcode, OUT,NOLOC,0};
						}
					} else {
						Location src = convertLocation(param1);
						if (src == NOLOC || src == OUT) {
							errorsExist = true;
							cout << "Unknown MOV source encountered on line " << i << endl;
							program[i] = {opcode,NOLOC,NOLOC,-1};
						} else {
							program[i] = {opcode,src,NOLOC,-1};
						}
					}
					front = ++delimiter;
					while (input[i][delimiter] != '\0') ++delimiter;
					Location dst = convertLocation(input[i].substr(front,delimiter-front));
					if (dst == NOLOC || dst == INP) {
						errorsExist = true;
						cout << "Unknown MOV destination encountered on line " << i << endl;
					}
					program[i].destination = dst;
				} else if (opcode == JRO) {
					bool isOnlyNumbers = true;
					for (int j = 0; j < delimiter-front; ++j) {
						if (!((param1[j] >= '0' && param1[j] <= '9') || param1[j] == '-')) isOnlyNumbers = false;
					}
					if (isOnlyNumbers) {
						if (stoi(param1) - i < 0 || stoi(param1) + i >= maxInstructions) {
							errorsExist = true;
							cout << "Unknown JRO immediate encountered on line " << i << endl;
							program[i] = {opcode,OUT,NOLOC,1};
						} else {
							program[i] = {opcode,OUT,NOLOC,static_cast<short>(stoi(param1))};
						}
					} else {
						Location src = convertLocation(param1);
						if (src == NOLOC || src == OUT) {
							errorsExist = true;
							cout << "Unknown JRO source encountered on line " << i << endl;
							program[i] = {opcode,NOLOC,NOLOC,1};
						} else {
							program[i] = {opcode,src,NOLOC,1};
						}
					}
				} else {
					errorsExist = true;
					cout << "Unknown parameter instruction encountered on line " << i << endl;
					program[i] = {UNKNOWN,NOLOC,NOLOC,-1};
				}
				break;
			}
			default: {
				errorsExist = true;
				cout << "Unexpected EOF on line " << i << endl;
			}
		}
		//debugPrintInstruction(program[i]);
	}
}

bool TIS100::push(short input) {
	if (stkSize < maxStkSize) {stk[stkSize++] = input; return true;}
	else cout << "Stack is full!" << endl; return false;
}

short TIS100::pop() {
	if (stkSize > 0) {return stk[--stkSize];}
	else cout << "Stack is empty!" << endl; return 0;
}

void TIS100::run(short input[], const int inputSize) {
	{ // ask for run confirmation
		if (errorsExist) {
			cout << "Your program has errors. Really continue? (Y/N) ";
			char response;
			do {
				cin >> response;
			} while (response != 'Y' && response != 'N');
			if (response == 'N') return;
		}
	}
	cout << "Run in debug mode? (Y/N) ";
	char response;
	do {
		cin >> response;
	} while (response != 'Y' && response != 'N');
	bool debugMode = false;
	if (response == 'Y') {debugMode = true; cout << "Debug mode requested. Step through the program by pressing any key.\n";}

	cout << endl << "Executing...\n";
	int inputCounter = 0;
	int outputCounter = 1;
	bool inputsRemaining = true;

	const int displayX = 0;
	const int displayY = 12;
	{ // static display
		consoleMove(displayX+31,displayY); cout << "OUTPUTS\n";
		consoleMove(displayX,displayY);
		cout << "-----------------------\nNext input:     \nACC:     \nBAK:     \n\n"; //positions should be (30,displayY+1), (30,displayY+2)
		for (int i = 0; i < numInstructions; ++i) {
			cout << "   " << i << '\t';
			if (labels[i] != "") cout << labels[i] << ':';
			printInstruction(program[i]);
			cout << '\n';
		}
		cout << "-----------------------\n";
	}
	while (inputsRemaining) {
		//print the display
		{
			consoleMove(displayX,displayY);
			for (int i = 0; i < numInstructions; ++i) {
				consoleMove(displayX,displayY+5+i);
				if (PC == i) {setColor(LIGHTRED,BLACK); cout << "-> "; setColor(LIGHTGRAY,BLACK);}
				else cout << "   ";
			}
			consoleMove(displayX+12,displayY+1);
			if (inputCounter < inputSize) {
				cout << input[inputCounter] << "    ";
			} else cout << "None";
			consoleMove(displayX+5,displayY+2); cout << acc << "    ";
			consoleMove(displayX+5,displayY+3); cout << bak << "    ";
		}
		if (debugMode) getch(); //pause on debug mode
		switch(program[PC].opcode) {
			case UNKNOWN:
			case NOP: break;
			case SWP: {
				short tempacc = acc;
				acc = bak;
				bak = tempacc;
				break;
			}
			case SAV: {bak = acc; break;}
			case NEG: {acc = -acc; break;}
			case ADD: {
				switch (program[PC].source) {
					case ACC: {acc += acc; break;}
					case INP: {
						if (inputCounter >= inputSize) {inputsRemaining = false; goto runExit;}
						else acc += input[inputCounter++];
						break;
					}
					case STK: {acc += pop(); break;}
					case OUT: {acc += program[PC].label; break;}
					default: {cout << "Invalid ADD source!\n";}
				}
				break;
			}
			case SUB: {
				switch (program[PC].source) {
					case ACC: {acc -= acc; break;}
					case INP: {
						if (inputCounter >= inputSize) {inputsRemaining = false; goto runExit;}
						else acc -= input[inputCounter++];
						break;
					}
					case STK: {acc -= pop(); break;}
					case OUT: {acc -= program[PC].label; break;}
					default: {cout << "Invalid SUB source!\n";}
				}
				break;
			}
			case MOV: {
				short value;
				switch(program[PC].source) {
					case ACC: {value = acc; break;}
					case INP: {
						if (inputCounter >= inputSize) {inputsRemaining = false; goto runExit;}
						else value = input[inputCounter++];
						break;
					}
					case STK: {value = pop(); break;}
					case OUT: {value = program[PC].label; break;}
					default: {cout << "Invalid MOV source!\n";}
				}
				switch(program[PC].destination) {
					case ACC: {acc = value; break;}
					case OUT: {consoleMove(displayX+31,displayY+outputCounter++); cout << value << '\n'; break;}
					case STK: {push(value); break;}
					default: {cout << "Invalid MOV destination!\n";}
				}
				break;
			}
			case JMP: {PC = program[PC].label; --PC; break;}
			case JEZ: {if (acc == 0) {PC = program[PC].label; --PC;} break;}
			case JNZ: {if (acc != 0) {PC = program[PC].label; --PC;} break;}
			case JGZ: {if (acc > 0) {PC = program[PC].label; --PC;} break;}
			case JLZ: {if (acc < 0) {PC = program[PC].label; --PC;} break;}
			case JRO: {
				switch (program[PC].source) {
					case ACC: {PC += acc; break;}
					case INP: {
						if (inputCounter >= inputSize) {inputsRemaining = false; goto runExit;}
						else PC += input[inputCounter++];
						break;
					}
					case STK: {PC += pop(); break;}
					case OUT: {PC += program[PC].label; break;}
					default: {cout << "Invalid JRO label!\n";}
				}
				--PC;
				break;
			}
			default: {cout << "Unknown instruction!\n";}
		}
		runExit:
		++PC;
		if (PC >= numInstructions) PC = 0;
	}
	consoleMove(0,40);
}

