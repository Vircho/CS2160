//dehack.c
//Author: Joshua White
//Class: CS2160
//Due: 10/13/24

//libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Constants
#define HACK_INST_SIZE 16

//Function Definitions

//Functions that open files
FILE* openFileR(char* filename);
FILE* openFileW(char* filename);

//Functions that manipulate a file or it's name
int countFileLines(char* filename);
char* removeFileExtension(const char* filename);

//Functions that translate hack to asm
void translateHackToASM(char* hackFile, char* filename, int filelength);
void atypeTranslate(char* instruction, char* translation);
void ctypeTranslate(char* instruction, char* translation);

//Functions that manipulate strings
void strfar(char* string, char find, char replace);

int main(int argc, char* argv[]) {

	//Variables
	
		//The amount of instructions in file
		int instCount = 0;

		//String representing the filename with the file extension
		char* filename;

		//String representing the file name without the file extension
		char* pureFileName;

	//Create the instructions file
	FILE* instFile = openFileR(argv[1]);

	//Set the instructions file name
	filename = argv[1];

	//Only move on if the file was successfully opened
	if (instFile != NULL) {

		//Close the file once it has been verified to exist
		fclose(instFile);

		//Find number of lines in file
		instCount = countFileLines(filename);

		//Create a purename for the file
		pureFileName = removeFileExtension(filename);
		
		//Translate the file
		translateHackToASM(filename, pureFileName, instCount);

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

//Function countFileLines - returns an int representing number of lines in file
int countFileLines(char* filename) {
	
	//Variables
	//Amount of lines in the file
	int lines = 0;
	//The character currently pointed at in the file
	char thisChar;

	//Open file to count lines in
	FILE* file = openFileR(filename);

	//Loop through the file
	while ( (thisChar = fgetc(file)) != EOF) {

		//Increment line count upon encountering a newline character
		if (thisChar == '\n') {
			lines++;
		}
	}

	//Since the loop does not count the final line
	//due to the final line of text files not having a newline, 
	//manually increment one more line to represent final line
	lines++;

	//Print number of lines found
	printf("Number of lines in file found: %d\n", lines);

	//Close the file when done
	fclose(file);

	return lines;
}

//Function removeFileExtension - removes the file extension from a file name & returns a string
//representing the filename sans the extension
char* removeFileExtension(const char* filename) {

	//allocate memory for the pureFlName string, assuming it's the same size as filename to start
	int flnameL = strlen(filename);
	char* pureFlName = (char*)malloc(flnameL * sizeof(char));

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

//Function translateHackToASM - Goes through a hack file with a certain filename and translates it to and writes the translation
//to a .asm file
void translateHackToASM(char* hackFile, char* filename, int filelength) {

	//Create a string to represent the asm file
	//Find how long the filename will be and allocate memory for it
	int lengthOfName = strlen(filename);
	int extLen = 4;
	int lengthOfFilename = lengthOfName + extLen;
	char* asmFilename = malloc(lengthOfFilename);

	//Create the filename
	strcpy(asmFilename, filename);
	strcat(asmFilename, ".asm");

	//Print filename
	printf("ASM filename made: %s\n", asmFilename);

	//Open both hack and asm files for reading & writing
	FILE* hack = openFileR(hackFile);
	FILE* ASM = openFileW(asmFilename);

	//Create variables for the procedure
	char thisChar = 'x'; //char to represent char being pointed to.
	bool eol = false; //bool to represent if at end of line
	int inc = 0; //int to represent where in the array is being written to
	char instruction[HACK_INST_SIZE + 1]; //string to represent one instruction (+1 for null-terminator)
	char translation[HACK_INST_SIZE + 1]; //string to represent the asm translation of the instruction (+1 for null-terminator)
	bool atype = false; //represents if the instruction is a-type
	bool ctype = false; //represents if the instruction is c-type

	while ((thisChar = fgetc(hack)) != EOF) {

		//End of line (eol) if theres a newline
		if (thisChar == '\n') {
			eol = true;
		}

		//If there's not a newline
		if (!eol) {

			//Continue filling the instruction array
			instruction[inc] = thisChar;
			inc++;
		}
		else {

			//Null-terminate the instruction array
			instruction[inc] = '\0';
			printf("instruction made: %s\n", instruction);

			//Check for instruction type
			if (instruction[0] == '0') {
				atype = true;
				ctype = false;
			}
			else if (instruction[0] == '1') {
				ctype = true;
				atype = false;
			}

			// Translate based on instruction type
			if (atype) {
				atypeTranslate(instruction, translation);
			}
			if (ctype) {
				ctypeTranslate(instruction, translation);
			}

			//Write the instruction translation to the file
			fprintf(ASM, "%s\n", translation);

			//Reset variables
			eol = false;
			inc = 0;
		}
	}

	//while loop does not move on after the last fill, so deal with final line
	instruction[inc] = '\0';
	printf("instruction made: %s\n", instruction);

	//check for type
	if (instruction[0] == '0') {

		atype = true;
		ctype = false;
	}
	else if (instruction[0] == '1') {

		ctype = true;
		atype = false;
	}

	if (atype) {

		atypeTranslate(instruction, translation);

	}
	if (ctype) {

		ctypeTranslate(instruction, translation);

	}
	
	//write to file
	fprintf(ASM, "%s\n", translation);

	//Close files
	fclose(hack);
	fclose(ASM);
}

//function atypeTranslate - Given a string representation of an atype 16 bit binary instruction
//and a string to write the translation to, translates from binary to hack ASM
void atypeTranslate(char* instruction, char* translation) {

	//Variables
	int pos = 1; //represents how much the current position is worth
	int decRep = 0; //represents the binary number in decimal
	int transIndex = 0; //represents which index of the translation array needs to be filled next

	//Loop through instruction and translate it as an integer number
	for (size_t i = 0; i < sizeof(instruction); i++) {

		if (instruction[i] == '1') {
			decRep += pos;
		}

		pos = pos * 2;
	}

	//Print the found value
	printf("A-type decimal representation found: %d\n", decRep);

	//Fill the translation

	//Since it's an atype instruction, there is a guaranteed @ at the front
	translation[0] = '@';
	transIndex++;

	//Convert the integer representing a decimal number into a string
	//Reason for 5 indexes is because the maximum number a 16-bit binary number can represent is 5 digits long
	char strRep[5];

	//sprintf - "string printf"
	//Takes in a string, a string to "print" to that string, and variables to replace the formaters.
	//Therefore, I am replacing %d with the decRep, causing sprintf to write the binary number in a string
	//format to the strRep

	//Added later - Check to see if SP is being used
	if (decRep == 0) {
		sprintf(strRep, "SP");
	}
	else {
		sprintf(strRep, "%d", decRep);
	}

	//It is not necessary to allocate more space in the translation array as it can hold up to 16 chars and the
	//translated atype instruction can be a maximum of 6 chars.

	for (int i = 0; strRep[i] != '\0'; i++) {
		translation[transIndex] = strRep[i];
		transIndex++;
	}

	//Null-terminate the string
	translation[transIndex] = '\0';
}

//Function ctypeTranslate - Given a string representation of a ctype 16 bit binary instruction
//and a string to write the translation to, translated from binary to hack ASM
void ctypeTranslate(char* instruction, char* translation) {

	//Variables
	
	//keeps track of which part of the c-instruction is being translated. starts at 3 as the first 3 (0-2)
	//don't get translated into anything.
	int index = 3;
	char reg = 'x'; //represents the register used (A or M)
	if (instruction[index] == '0') {
		reg = 'A';
	}
	else if (instruction[index] == '1') {
		reg = 'M';
	}

	//Increment index because the value of reg section has been saved
	index++;

	//Computation, use two arrays of strings and compare
	//Binary versions of the compuations
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


	//Now that they're made, relate the binCompVer to the bits in the computation segment of instruction (4-9)
	char compBits[7];

	//fill compBits with the comp section of instruction
	for (int i = 0; i < 6; i++) {

		compBits[i] = instruction[index];
		index++;
	}

	//Null-terminate because I am having so many issues with null-termination of strings in this code and I need
	//to start null-terminating everything
	compBits[6] = '\0';

	// Compare the comp-bits to binCompVer array to find which comp argument is being used
	int compIndex = 999; // the index of where in the array the correct computation is
	//Find how many possible computations there can be. This could technically be hardcoded as 18 as I did when
	//making the table, but I want to avoid hardcoding where I can
	int compPos = sizeof(binCompVer) / sizeof(binCompVer[0]);
	for (int i = 0; i < compPos; i++) {

		if (strcmp(compBits, binCompVer[i]) == 0) {
			compIndex = i; 
		}
	}

	if (compIndex == 999) {
		puts("Error in comparing computation bits");
	}

	//Set a character to represent the final computation
	char comp[HACK_INST_SIZE] = "";
	strcpy(comp, asmCompVer[compIndex]);

	//Replace the R with the correct register
	strfar(comp, 'R', reg);

	//Next, find the dest
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

	//Create string to hold destBits
	char destBits[4];

	//Fill destBits with the dest parts of the instruction
	for (int i = 0; i < 3; i++) {

		destBits[i] = instruction[index];
		index++;

	}

	//Null-terminate
	destBits[3] = '\0';

	//Compare destBits to the tables
	int destIndex = 999;
	int destPos = sizeof(binDestVer) / sizeof(binDestVer[0]);

	for (int i = 0; i < destPos; i++) {

		if ((strcmp(destBits, binDestVer[i])) == 0) {
			destIndex = i;
		}
	}

	if (destIndex == 999) {
		puts("Error in comparing destination bits");
	}

	char dest[HACK_INST_SIZE] = "";
	strcpy(dest, asmDestVer[destIndex]);

	//Deal with jump bits
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

	//Create string to hold jump bits
	char jumpBits[4];

	//Fill destBits with the jump parts of the instruction
	for (int i = 0; i < 3; i++) {

		jumpBits[i] = instruction[index];
		index++;

	}

	//Null-terminate
	jumpBits[3] = '\0';

	//Compare destBits to the tables
	int jumpIndex = 999;
	int jumpPos = sizeof(binJumpVer) / sizeof(binJumpVer[0]);

	for (int i = 0; i < jumpPos; i++) {

		if ((strcmp(jumpBits, binJumpVer[i])) == 0) {
			jumpIndex = i;
		}
	}

	if (jumpIndex == 999) {
		puts("Error in comparing jump bits");
	}

	char jump[HACK_INST_SIZE] = "";
	strcpy(jump, asmJumpVer[jumpIndex]);

	//Print the final comp dest and jump
	printf("Comp: %s\n", comp);
	printf("Dest: %s\n", dest);
	printf("Jump: %s\n", jump);

	//Put them together in one string

	//Create some variables to keep track of lengths
	int transIndex = 0;
	int compLength = strlen(comp);
	int destLength = strlen(dest);
	int jumpLength = strlen(jump);

	//Check if jump
	if (strcmp(jump, "null") == 0) {
		if (strcmp(dest, "null") == 0) {

			//Write comp
			for (int i = 0; i < compLength; i++) {
				translation[transIndex] = comp[i];
				transIndex++;
			}

			//Write the null-terminator to it
			translation[transIndex] = '\0';

		}
		else {

			//Write dest
			for (int i = 0; i < destLength; i++) {
				translation[transIndex] = dest[i];
				transIndex++;
			}

			//Write =
			translation[transIndex] = '=';
			transIndex++;

			//Write comp
			for (int i = 0; i < compLength; i++) {
				translation[transIndex] = comp[i];
				transIndex++;
			}

			//Write the null-terminator to it
			translation[transIndex] = '\0';
		}
	}
	else {

		//I could have used strcat() here, but I decided that this method would be better, as it gives me more control.
		//Add on the comp
		for (int i = 0; i < compLength; i++) {
			translation[transIndex] = comp[i];
			transIndex++;
		}

		//Add on the ;
		translation[transIndex] = ';';
		transIndex++;

		//Add on the jump
		for (int i = 0; i < jumpLength; i++) {
			translation[transIndex] = jump[i];
			transIndex++;
		}

		//Null-terminate translation
		translation[transIndex] = '\0';

	}

	//print the translation
	printf("ctype translation: %s\n", translation);

}