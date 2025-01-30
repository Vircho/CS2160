//assemble.c
//author: Joshua White
//Class: CS2160
//Due: 10/13/24

//libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Constants
#define HACK_INST_SIZE 256 //changed from 16 due to it being possible for registers to be larger than 16

//Function definition

//Functions that open files
FILE* openFileR(char* filename);
FILE* openFileW(char* filename);

//Functions that manipulate or gain information from files
char* removeFileExtension(const char* filename);

//Functions that translate
void translateASMtoHack(char* fromFile, char* pureFilename);
void atypeTranslator(char* instruction, char* translation, bool isSymbolic, char** symbol, char** meaning, int sizeofTable);
void ctypeTranslator(char* instruction, char* translation);

//Functions that build the symbol table
void fillSymbolTables(char* filename, char** calls, char** labels, char** lineNums, int* CTS, int* LTS, int* NTS);
int numberOfCalls(char* filename);
int numberOfLabels(char* filename);
void trimSymbols(char* instruction, char* symbol);
void linkSymbols(char** calls, char** labels, char** lines, char** meanings, int callN, int lablN);

//Other functions
bool isSymbolicReg(char* regInst);
void decToBin(char* dec, char* bin);
void strfar(char* string, char find, char replace);
void writeSymbolTables(char** symbols, char** meanings, char* filename, int STS);

int main(int argc, char* argv[]) {

	//Variables

	//String representing the filename with the file extension
	char* filename;

	//String representing the file name without the file extension
	char* pureFilename;

	//Create the instructions file
	FILE* instFile = openFileR(argv[1]);

	//Set the instructions file name
	filename = argv[1];

	//Only move on if the file was successfully opened
	if (instFile != NULL) {

		//Close the file once it has been verified to exist
		fclose(instFile);

		//Create a purename for the file
		pureFilename = removeFileExtension(filename);

		//Translate the file
		translateASMtoHack(filename, pureFilename);
	}

}

//Function openFileR - opens a file with a string of the file path for reading
//Returns an error if cannot open file
FILE* openFileR(char* filename) {

	//Create pointer to file
	FILE* file;

	//Attempt to open the file
	file = fopen(filename, "r");

	if (file == NULL) {
		puts("Error in opening file for reading - file is NULL");
	}

	return file;

}

//Function openFileW = opens a file with a string of the file path for writing
//Returns an error if cannot open file
FILE* openFileW(char* filename) {

	FILE* file;
	file = fopen(filename, "w");

	if (file == NULL) {
		puts("Error in opening a file for writing - file is NULL");
	}

	return file;
}

//Function removeFileExtension - removes the file extension from a file name & returns a string
//representing the filename sans the extension
char* removeFileExtension(const char* filename) {

	//allocate memory for the pureFlName string, assuming it's the same size as filename to start
	char* pureFlName = (char*)malloc(strlen(filename));

	//boolean value representing whether the pureFlName string should still be written to
	bool writingEnd = false;

	int nameLength = strlen(filename);
	//Go until the end of the filename string
	for (int i = 0; i < nameLength; i++) {

		//If there's a '.'
		if (filename[i] == '.') {

			//Null-terminate the pureFlName string
			pureFlName[i] = '\0';

			//Stop the writing
			writingEnd = true;
		}

		//As long as there's still writing going on, continue writing.
		if (!writingEnd) {
			pureFlName[i] = filename[i];
		}
	}

	//Print the pure file name
	printf("Pure Filename Made: %s\n", pureFlName);

	return pureFlName;
}

//Function translateASMtoHack - Given a file that contains asm code, translates to hack binary
void translateASMtoHack(char* fromFile, char* pureFilename) {

	//Create a string to represent the asm file
	//Find how long the filename will be and allocate memory for it
	int lengthOfName = strlen(pureFilename);
	int extLen = 7;
	int lengthOfFilename = lengthOfName + extLen;
	char* hackFilename = (char*) malloc(lengthOfFilename * sizeof(char));

	//Create the filename
	strcpy(hackFilename, pureFilename);
	strcat(hackFilename, "-bn.txt");

	//Print filename
	printf("Hack filename made: %s\n", hackFilename);

	//Find how many calls to a label or loading of a custom-named register there are
	int calls = numberOfCalls(fromFile);
	int labels = numberOfLabels(fromFile);

	//Allocate memory for two tables + strings representing which line is directly underneath each label + table represening meanings
	char** symbolicCalls = (char**)malloc(calls * sizeof(char*));

	char** labelNames = (char**)malloc(labels * sizeof(char*));
	char** lineNumbers = (char**)malloc(labels * sizeof(char*));

	char** symbolicMeanings = (char**)malloc(calls * sizeof(char*));

	//I realized that the numbers aren't entirely correct, so create some more accurate ones
	int callTableSize = 0;
	int labelTableSize = 0;
	int lineTableSize = 0;

	//Create the symbol table and link them with numbers
	fillSymbolTables(fromFile, symbolicCalls, labelNames, lineNumbers, &callTableSize, &labelTableSize, &lineTableSize);

	//Finally, link the symbolic calls to either line numbers or registers
	linkSymbols(symbolicCalls, labelNames, lineNumbers, symbolicMeanings, calls, labels);

	//Print the symbol tables to a file
	writeSymbolTables(symbolicCalls, symbolicMeanings, pureFilename, callTableSize);

	//Now I can move through the file
	//Open the file
	FILE* asmFile = openFileR(fromFile);
	FILE* hackFile = openFileW(hackFilename);

	//variables
	char thisChar; //represents the char being looked at
	bool comment = false; //represents if the line could be a comment
	bool eol = false; //represents if the line is at a newline
	char instruction[HACK_INST_SIZE]; //represents one instruction
	char translation[HACK_INST_SIZE]; //represents the translation of an instruction
	int instInc = 0; //increments the index of instruction

	bool isSymbolic = false;

	//Go through the file
	while ((thisChar = fgetc(asmFile)) != EOF) {

		//Line is at end of line if there's a newline
		if (thisChar == '\n') {
			eol = true;
		}

		//Move on if there's a tab or space
		if (thisChar != '\t' && thisChar != ' ') {

			//Check to see if the line is or has become a comment
			if (thisChar == '/') {
				comment = true;
			}

			//As long as it's not a comment, it's a valid line, continue writing as long as it's not the end of line
			if (!comment && !eol) {

				instruction[instInc] = thisChar;
				instInc++;

			}
		}

		//If at the end of the line
		if (eol) {

			//Null-terminate
			instruction[instInc] = '\0';

			if (instruction[0] != '\0') {

				printf("Valid instruction made: %s\n", instruction);

				if (instruction[0] == '@') {
					isSymbolic = isSymbolicReg(instruction);
					atypeTranslator(instruction, translation, isSymbolic, symbolicCalls, symbolicMeanings, callTableSize);
				}
				else if (instruction[0] != '(') {
					ctypeTranslator(instruction, translation);
				}

				//Write the instruction to the file
				fprintf(hackFile, "%s\n", translation);
				
			}

			eol = false;
			comment = false;
			instInc = 0;
		
		}

	}

	//Close the files
	fclose(hackFile);
	fclose(asmFile);
}

//Function fillSymbolTables - Takes 3 tables for calls, labels, and line numbers in a filename, and fills them
void fillSymbolTables(char* filename, char** calls, char** labels, char** lineNums, int* CTS, int* LTS, int* NTS) {

	//Open the file
	FILE* file = openFileR(filename);

	//Variables
	char thisChar; //Represents the char being looked at
	bool comment = false; //Represents if the line could be a comment
	bool eol = false; //Represents if the line is at a newline
	char instruction[HACK_INST_SIZE]; //Represents one instruction
	int instInc = 0; //Increments the index of instruction

	//Increments the call and label tables
	int thisCall = 0;
	int thisLabel = 0;
	int nextLine = 1;

	//Go through the file
	while ((thisChar = fgetc(file)) != EOF) {

		//Line is at end of line if there's a newline
		if (thisChar == '\n') {
			eol = true;
		}

		//Move on if there's a tab or space
		if (thisChar != '\t' && thisChar != ' ') {

			//Check to see if the line is or has become a comment
			if (thisChar == '/') {
				comment = true;
			}

			//As long as it's not a comment, it's a valid line, continue writing as long as it's not the end of line
			if (!comment && !eol) {
				instruction[instInc] = thisChar;
				instInc++;
			}
		}

		//If at the end of the line
		if (eol) {

			//Null-terminate the instruction
			instruction[instInc] = '\0';

			if (instruction[0] != '\0') {

				printf("Valid instruction found: %s\n", instruction);

				//If it's a call or a load, check to see if it's custom
				if (instruction[0] == '@') {

					bool isSymbol = isSymbolicReg(instruction);

					//If it is a symbolic register add it to the calls table
					if (isSymbol) {

						puts("This instruction is a symbolic call");

						//Get the symbol without @
						char symbol[HACK_INST_SIZE];
						trimSymbols(instruction, symbol);

						//Allocate memory for the string and copy it
						int callLength = strlen(symbol);
						calls[thisCall] = (char*)malloc(callLength * (sizeof(char)));
						strcpy(calls[thisCall], symbol);

						printf("Added to calls: %s\n", calls[thisCall]);
						thisCall++;

						//Increment the Calls Table Size every time a new call gets added
						CTS++;
					}
				}

				//If it's a label, add it to the labels table
				if (instruction[0] == '(') {
					puts("This instruction is a label");

					//Get the symbol without ()
					char symbol[HACK_INST_SIZE];
					trimSymbols(instruction, symbol);

					//Allocate memory for the string and copy it
					int labLength = strlen(symbol);
					labels[thisLabel] = (char*)malloc(labLength * (sizeof(char)));
					strcpy(labels[thisLabel], symbol);

					printf("Added to labels: %s\n", labels[thisLabel]);
					thisLabel++;

					//Add the line below this label to the lineNums table
					//Convert nextLine to a char
					char charLine[HACK_INST_SIZE];
					sprintf(charLine, "%d", nextLine);

					int charLineSize = strlen(charLine);

					//Add to lineNums - I can use thisLabel as every line number is paired with a label
					lineNums[thisLabel] = (char*)malloc(charLineSize * sizeof(char));
					strcpy(lineNums[thisLabel], charLine);

					printf("Added to lineNums: %s\n", lineNums[thisLabel]);

					//Increment the Labels Table Size every time a new call gets added
					LTS++;

					//Increment the line Numbers Table Size every time a new call gets added
					NTS++;
				}
				else {
					nextLine++;
				}

			}

			eol = false;
			comment = false;
			instInc = 0;
		}
	}

	//Close the file
	fclose(file);
}

//Function atypeTranslator - Translated an atype instruction in asm to binary
void atypeTranslator(char* instruction, char* translation, bool isSymbolic, char** symbol, char** meaning, int sizeofTable) {

	//Variables
	char realReg[HACK_INST_SIZE];
	char binaryVer[17];

	//Start by adding a '0' to the beginning
	translation[0] = '0';

	//Null-terminate because I'm using strcat later
	translation[1] = '\0';

	//Trim the parts off the instruction
	char nakedInstruction[HACK_INST_SIZE];
	trimSymbols(instruction, nakedInstruction);

	//Check to see if it's a symbolic
	if (isSymbolic) {

		//If it's symbolic, find what it's meaning is
		for (int i = 0; i < sizeofTable; i++) {

			if (strcmp(nakedInstruction, symbol[i]) == 0) {
				strcpy(realReg, meaning[i]);
			}
		}

		decToBin(realReg, binaryVer);
	}
	else {
		decToBin(nakedInstruction, binaryVer);
	}

	//Add on the binary: Normally, I wouldn't use strcat for this, but I'll make an exception to simplify this process.
	//In the future, I may rewrite this if I have reason to.
	strcat(translation, binaryVer);

}

//Function ctypeTranslator - Translates a ctype translation in asm to binary
void ctypeTranslator(char* instruction, char* translation) {

	//Variables
	//Size of instruction
	int instSize = strlen(instruction);

	//Is dest null
	bool destNull = true;

	//Binary representations parts
	char destB[HACK_INST_SIZE] = "";
	char compB[HACK_INST_SIZE] = "";
	char jumpB[HACK_INST_SIZE] = "";
	char regB[HACK_INST_SIZE] = "";

	//Does the instruction jump or comp
	char jorc = 'x';

	//Since it's a c-type instruction the first 3 bits are guarenteed to be 1
	int index = 3;
	translation[0] = '1';
	translation[1] = '1';
	translation[2] = '1';

	//Unlike the dehack, I'll start with dest as that's what the first part of the hack instruction is
	char dest[5];

	//Detect null by checking for equal sign
	for (int i = 0; i < instSize; i++) {

		if (instruction[i] == '=') {
			destNull = false;
		
			jorc = 'c';
		}
		else if (instruction[i] == ';') {
			jorc = 'j';
		}

	}
	
	if (destNull == true) {
		strcpy(dest, "null");
	}

	//If there's a dest, read up until the =
	bool hitEq = false;
	int destIndex = 0;
	if (destNull == false) {

		for (int i = 0; i < instSize; i++) {

			if (instruction[i] == '=') {
				hitEq = true;
			}

			if (!hitEq) {
				dest[destIndex] = instruction[i];
				destIndex++;
			}

		}

		//Null-terminate
		dest[destIndex] = '\0';

	}

	//Tables for dest bits
	char* binDestVer[8] = {
		"000", //null
		"001", //M
		"010", //D
		"011", //DM
		"100", //A
		"101", //AM
		"110", //AD
		"111", //ADM
	};
	char* asmDestVer[8] = {
		"null", //000
		"M", //001
		"D", //010
		"DM", //011
		"A", //100
		"AM", //101
		"AD", //110
		"ADM", //111
	};

	//Compare dest instruction to the tables
	destIndex = 999; //reusing because I can't think of names to seperate these two index-incrementers
	int destPos = sizeof(asmDestVer) / sizeof(asmDestVer[0]);


	for (int i = 0; i < destPos; i++) {

		if ((strcmp(dest, asmDestVer[i])) == 0) {
			destIndex = i;
		}
	}

	if (destIndex == 999) {
		puts("Error in comparing destination bits");
	}


	strcpy(destB, binDestVer[destIndex]);

	printf("Binary version of dest found as %s\n", destB);

	//Next, translate either the jump or the comp
	if (jorc == 'c') {

		//No jump
		strcpy(jumpB, "000");

		//String to represent comp section
		char comp[4];

		//Comp tables
		char* binCompVer[18] = {
			"101010", //0
			"111111", //1
			"111010", //-1
			"001100", //D
			"110000", //R
			"001101", //!D
			"110001", //!R
			"001111", //-D
			"110011", //-R
			"011111", //D+1
			"110111", //R+1
			"001110", //D-1
			"110010", //R-1
			"000010", //D+R
			"010011", //D-R
			"001111", //R-D
			"000000", //D&R
			"010101", //D|R
		};
		char* asmCompVer[18] = {
			"0",      //101010
			"1",      //111111
			"-1",     //111010
			"D",      //001100
			"R",      //110000
			"!D",     //001101
			"!R",     //110001
			"-D",     //001111
			"-R",     //110011
			"D+1",    //011111
			"R+1",    //110111
			"D-1",    //001110
			"R-1",    //110010
			"D+R",    //000010
			"D-R",    //010011
			"R-D",    //001111
			"D&R",    //000000
			"D|R"     //010101
		};

		//Fill string with comp section
		bool inComp = false;
		int compIndex = 0;
		char reg = 'x';
		for (int i = 0; i < instSize; i++) {

			if (inComp) {
				comp[compIndex] = instruction[i];
				compIndex++;
			}

			//While looking at comp, see if it's using A or M
			if (inComp) {

				if (instruction[i] == 'A') {
					reg = 'A';
					strcpy(regB, "0");
				}
				else if (instruction[i] == 'M') {
					reg = 'M';
					strcpy(regB, "1");
				}
			}

			if (instruction[i] == '=') {
				inComp = true;
			}


		}//end of forloop

		printf("Reg bits found as: %s\n", regB);

		//Null-terminate
		comp[compIndex] = '\0';

		//Replace comp with R for comparing
		if (reg == 'A') {
			strfar(comp, 'A', 'R');
		}
		else if (reg == 'M') {
			strfar(comp, 'M', 'R');
		}

		//Compare comp with the tables
		compIndex = 999; 
		
		int compPos = sizeof(asmCompVer) / sizeof(asmCompVer[0]);
		for (int i = 0; i < compPos; i++) {

			if (strcmp(comp, asmCompVer[i]) == 0) {
				compIndex = i;
			}
		}

		if (compIndex == 999) {
			puts("Error in comparing computation bits");
		}

		strcpy(compB, binCompVer[compIndex]);

		printf("Binary version of comp found as %s\n", compB);

	}
	else if (jorc == 'j') {

		//No comp
		strcpy(compB, "000000");

		//String to represent comp section
		char jump[4];

		//Comp tables
		char* binJumpVer[8] = {
			"000", //null
			"001", //JGT
			"010", //JEQ
			"011", //JGE
			"100", //JLT
			"101", //JNE
			"110", //JLE
			"111", //JMP
		};
		char* asmJumpVer[8] = {
			"null", //000
			"JGT", //001
			"JEQ", //010
			"JGE", //011
			"JLT", //100
			"JNE", //101
			"JLE", //110
			"JMP", //111
		};

		//Fill string with comp section
		bool inJump = false;
		int jumpIndex = 0;
		for (int i = 0; i < instSize; i++) {

			if (inJump) {
				jump[jumpIndex] = instruction[i];
				jumpIndex++;
			}

			if (instruction[i] == ';') {
				inJump = true;
			}

		}//end of forloop

		//Null-terminate
		jump[jumpIndex] = '\0';


		//Compare jump with the tables
		jumpIndex = 999;

		int compPos = sizeof(asmJumpVer) / sizeof(asmJumpVer[0]);
		for (int i = 0; i < compPos; i++) {

			if (strcmp(jump, asmJumpVer[i]) == 0) {
				jumpIndex = i;
			}
		}

		if (jumpIndex == 999) {
			puts("Error in comparing computation bits");
		}

		strcpy(jumpB, binJumpVer[jumpIndex]);

		printf("Binary version of jump found as %s\n", jumpB);

	}//end of jump or comp

	//Finally, put all the bit strings together	
	
	int compSize = strlen(compB);
	int destSize = strlen(destB);
	int jumpSize = strlen(jumpB);
	
	//Somehow, this didn't work, and I couldn't figure out why, so I'm going to have to hardcode it for now
	/*
	* translation[index] = regB[1];
	* index++;

	for (int i = 0; i < compSize; i++) {
		translation[index] = compB[i];
		index++;
	}
	for (int i = 0; i < destSize; i++) {
		translation[index] = destB[i];
		index++;
	}
	for (int i = 0; i < jumpSize; i++) {
		translation[index] = jumpB[i];
		index++;
	}
	
	*/

	/*
	* Somehow, not even this worked ?
	* translation[4] = regB[1];
	translation[5] = compB[0];
	translation[6] = compB[1];
	translation[7] = compB[2];
	translation[8] = compB[3];
	translation[9] = compB[4];
	translation[10] = compB[5];
	translation[11] = destB[0];
	translation[12] = destB[1];
	translation[13] = destB[2];
	translation[14] = jumpB[0];
	translation[15] = jumpB[1];
	translation[16] = jumpB[2];
	*/

	//This didn't work either.
	strcat(translation, regB);
	strcat(translation, compB);
	strcat(translation, destB);
	strcat(translation, jumpB);
	

}

//Function decToBin - takes a string representing a decimal number and converts it to a 16-bit binary number,
//storing in another string Note: While the more general use of this would be as 16-bit I modified it for 15-bit
//for the purposes of this code
void decToBin(char* dec, char* bin) {

	//Variables
	int place = 16384; //Represents the place in the binary number, starts at 
	int binInc = 0;

	//Convert the string to an int
	char* end;
	double doubleDec = strtod(dec, &end);
	int intDec = (int)doubleDec;

	//Convert it to binary
	while (place != 0) {

		if (place > intDec) {
			bin[binInc] = '0';
			binInc++;
		}
		else {
			bin[binInc] = '1';
			binInc++;
			intDec = intDec - place;
		}

		place = place / 2;
	}

	//Null-terminate
	bin[binInc] = '\0';

	printf("Final binary number: %s\n", bin);
}

//Function linkSybols - Links up two symbol tables
void linkSymbols(char** calls, char** labels, char** lines, char** meanings, int callN, int lablN) {

	bool iLinked = false; //Represents if the symbolic call is a label call
	int nextAvailReg = 16; //Represents which register is not yet taken up (starts at 16)

	//Move through all calls
	for (int i = 0; i < callN; i++) {

		//For each call, move through all labels
		for (int j = 0; j < lablN; j++) {

			//Compare each call with each label
			if (strcmp(calls[i], labels[j]) == 0) {

				iLinked = true;
				meanings[i] = lines[j];

			}

		}

		//Set meaning to a register
		if (iLinked == false) {

			//Make nextAvailReg a string
			char charRep[HACK_INST_SIZE];
			sprintf(charRep, "%d", nextAvailReg);
			meanings[i] = charRep;
			nextAvailReg++;

		}
	}
}

//Function isSymbolicReg - Returns a boolean value based on whether a string with the format @xxx... is symbolic or not
//On whether xxx are numbers or not
bool isSymbolicReg(char* regInst) {
	int instLength = strlen(regInst);
	char reg[HACK_INST_SIZE];
	bool symbol = false;
	int index = 0;

	//Fill reg with the register being called by excluding @
	for (int i = 1; i < instLength; i++) {
		reg[index] = regInst[i];
		index++;
	}

	//Null-terminate
	reg[index] = '\0';

	//Use strtod to test if the register is a string or numerical, as strtod fails if the string is not numerical 
	char* end;
	strtod(reg, &end);

	if (reg == end || *end != '\0') {
		symbol = true;
	}
	else {
		symbol = false;
	}

	//Check just in case it's @SP
	if (strcmp(regInst, "@SP") == 0) {
		symbol = false;
	}

	return symbol;
}

//Function numberOfCalls - finds how many calls to a custom register are in a hack assembly file
int numberOfCalls(char* filename) {

	//Open the file
	FILE* file = openFileR(filename);

	//variables
	char thisChar; //represents the char being looked at
	bool comment = false; //represents if the line could be a comment
	bool eol = false; //represents if the line is at a newline
	char instruction[HACK_INST_SIZE]; //represents one instruction
	int instInc = 0; //increments the index of instruction

	int calls = 0; //amount of instructions in the form @xxx where xxx is not a number or SP

	//Go through the file
	while ((thisChar = fgetc(file)) != EOF) {

		//Line is at end of line if there's a newline
		if (thisChar == '\n') {
			eol = true;
		}

		//Move on if there's a tab or space
		if (thisChar != '\t' && thisChar != ' ') {

			//Check to see if the line is or has become a comment
			if (thisChar == '/') {
				comment = true;
			}
			
			//As long as it's not a comment, it's a valid line, continue writing as long as it's not the end of line
			if (!comment && !eol) {
				
				instruction[instInc] = thisChar;
				instInc++;

			}
		}

		//If at the end of the line
		if (eol) {
			
			//Null-terminate the instruction
			instruction[instInc] = '\0';

			if (instruction[0] != '\0') {
				printf("Valid instruction found: %s\n", instruction);

				//If it's a call or a load, check to see if it's custom
				if (instruction[0] == '@') {

					bool isSymbol = isSymbolicReg(instruction);

					//If it is a symbolic register, increment the amount of calls
					if (isSymbol) {
						puts("This instruction is a symbolic call");
						calls++;
					}
				}

			}

			eol = false;
			comment = false;
			instInc = 0;

		}
	}

	//Print number of calls
	printf("Found %d symbolic calls\n", calls);

	//Close the file
	fclose(file);

	return calls;
}

//Function numberOfLabels - finds how many calls to a custom register are in a hack assembly file
int numberOfLabels(char* filename) {

	//Open the file
	FILE* file = openFileR(filename);

	//variables
	char thisChar; //represents the char being looked at
	bool comment = false; //represents if the line could be a comment
	bool eol = false; //represents if the line is at a newline
	char instruction[HACK_INST_SIZE]; //represents one instruction
	int instInc = 0; //increments the index of instruction

	int labels = 0; //amount of labels

	//Go through the file
	while ((thisChar = fgetc(file)) != EOF) {

		//Line is at end of line if there's a newline
		if (thisChar == '\n') {
			eol = true;
		}

		//Move on if there's a tab or space
		if (thisChar != '\t' && thisChar != ' ') {

			//Check to see if the line is or has become a comment
			if (thisChar == '/') {
				comment = true;
			}

			//As long as it's not a comment, it's a valid line, continue writing as long as it's not the end of line
			if (!comment && !eol) {

				instruction[instInc] = thisChar;
				instInc++;

			}
		}

		//If at the end of the line
		if (eol) {

			//Null-terminate the instruction
			instruction[instInc] = '\0';

			if (instruction[0] != '\0') {
				printf("Valid instruction found: %s\n", instruction);

				//If it's a label, increment labels
				if (instruction[0] == '(') {
					puts("This instruction is a label");
					labels++;
				}

			}

			eol = false;
			comment = false;
			instInc = 0;

		}
	}

	printf("Found %d labels\n", labels);

	//Close the file
	fclose(file);

	return labels;
}

//Function trimSymbols - removes @ or () from a symbolic instruction and stores it in symbol
void trimSymbols(char* instruction, char* symbol) {

	int instructionSize = strlen(instruction);

	int instInc = 0;

	if (instruction[0] == '@') {

		for (int i = 0; i < instructionSize; i++) {

			if (instruction[i] != '@') {
				symbol[instInc] = instruction[i];
				instInc++;
			}
		}

	}
	else if (instruction[0] == '(') {

		for (int i = 0; i < instructionSize; i++) {

			if (instruction[i] != '(' && instruction[i] != ')') {
				symbol[instInc] = instruction[i];
				instInc++;
			}

		}
	}

	symbol[instInc] = '\0';
}

//Function strfar "string find and replace" - replaces all instances of one character with another character in a string
void strfar(char* string, char find, char replace) {

	//Set up char to represent current character in string
	char thisChar = 'x';

	//Go through string until hits null-terminator
	for (int i = 0; string[i] != '\0'; i++) {

		thisChar = string[i];

		if (thisChar == find) {
			string[i] = replace;
		}

	}

}

//Function writeSymbolTables - writes two tables to a file
void writeSymbolTables(char** symbols, char** meanings, char* filename, int STS) {

	//Create the filename
	char sysFile[HACK_INST_SIZE];
	strcpy(sysFile, filename);
	strcat(sysFile, "-Symbols.sym");

	//Open the file
	FILE* file = openFileW(sysFile);

	//Write to the file
	fputs(file, "SYMBOLS	MEANINGS");
	for (int i = 0; i < STS; i++) {

		fprintf(file, "%s\t%s", symbols[i], meanings[i]);

	}
}