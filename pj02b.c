//pj02b.c
//author: Joshua White
//Class: CS2160
//Due: 11/17/24

//libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Constants
#define MAX_STRING_SIZE 32

//Function Definitions

//File Functions
FILE* openFileR(char* filename);
FILE* openFileW(char* filename);

//Conversion Functions
int repeatedMultiplication(char* num, int base, char* table);
void repeatedDivision(char* num, int base, char* table, char* output);

//String Functions
bool findString(const char* string, const char* toFind);
int simplerStrtol(char* string);
void strmrr(char* string, char* mirrored, unsigned int copy);


//Function main - main logic
int main(int argc, char* argv[]) {

	//Create the conversion table
	char conTable[65] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '#', '$', '\0'
	};

	//Open the files
	FILE* instructions = openFileR("pj02.dat");
	FILE* outputFile = openFileW("pj02.out");

	//Create strings to hold the instructions
	char num[MAX_STRING_SIZE];
	char fm[MAX_STRING_SIZE];
	char to[MAX_STRING_SIZE];

	//Create data variables
	long sum = 0;					//Sum of all decimal representations
	int decRep = 0;					//A decimal representation of one number in a base
	char baseRep[MAX_STRING_SIZE];	//The representation of a value in a base
	int base = 0;					//Special base (convert from if RM convert to if RD) 
	char decAsString[32];			//The representation of a value in base 10 as a string

	char thisChar = 'x';			//Current character being looked at
	unsigned int sec = 1;			//the section of instructions being looked at (num, fm, to)
	unsigned int counter = 0;		//General incrementer.

	//Begin printing the table
	fprintf(outputFile, "Input                           FM      TO      Decimal          Result\n");
	fprintf(outputFile, "---------------------------------------------------------------------------\n");


	//Begin loop through full file
	while ((thisChar = fgetc(instructions)) != EOF) {

		//Switch sections at each instance of ,
		if (thisChar == ',') {
			
			//Null-terminate appropriate string to close off one section before opening another
			switch (sec) {
			case 1:
				num[counter] = '\0';
				break;
			case 2:
				fm[counter] = '\0';
				break;
			}

			//Change section
			sec++;
			counter = 0;
		}

		//If newline, do the conversion
		else if (thisChar == '\n') {

			//End the final section
			to[counter] = '\0';
			
			//Decide whether to do repeated division, repeated multiplication, or both
			if (findString(to, "10")) {			//Do one instance of repeated multiplication

				//Create the base
				base = simplerStrtol(fm);

				//Create the base 10 representation
				decRep = repeatedMultiplication(num, base, conTable);

				//Set the base 10 representation as a string
				sprintf(decAsString, "%d", decRep);

				//Set the main base representation to the base 10 representation
				strcpy(baseRep, decAsString);

			}
			else if (findString(fm, "10")) {	//Do one instance of repeated division

				//Create the base
				base = simplerStrtol(to);

				//Make value converting from an int (utilize decRep)
				decRep = simplerStrtol(num);

				//Set the base 10 representation as a string
				sprintf(decAsString, "%d", decRep);

				//Do the conversion
				repeatedDivision(num, base, conTable, baseRep);

			}
			else {								//Do repeated multiplication followed by repeated division

				//Create the base
				base = simplerStrtol(fm);

				//Create a base 10 representation
				decRep = repeatedMultiplication(num, base, conTable);

				//Save the base 10 representation as a string
				sprintf(decAsString, "%d", decRep);

				//Create the next base
				base = simplerStrtol(to);

				//Do the conversion
				repeatedDivision(decAsString, base, conTable, baseRep);
			}

			//Increment the sum
			sum += decRep;

			//Reset
			sec = 1;
			counter = 0;

			//Test: Manual Printing
			fprintf(outputFile, "%-25s\t%-5s\t%-5s\t%-15s\t%-10s\n", num, fm, to, decAsString, baseRep);

		}

		//Otherwise, fill a section
		else {

			if (sec == 1) {
				num[counter] = thisChar;
			}
			else if (sec == 2) {
				fm[counter] = thisChar;
			}
			else if (sec == 3) {
				to[counter] = thisChar;
			}

			counter++;
		}
	}

	//After all numbers are read and converted, print the sum
	fprintf(outputFile, "-------------------------------------------------------------------\n");
	fprintf(outputFile, "                                                 Total: %ld\n", sum);

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

//Function findString - takes in a string and another string to find in it - returns true if the toFind string exists in the original string
bool findString(const char* string, const char* toFind) {

	//Variable to return
	bool isIn = false;

	//Lengths of the strings
	int strSize = strlen(string);
	int tofSize = strlen(toFind);

	//Is tofSize > strSize
	bool tooBig = false;

	//Incrementor of toFind - rises while potentially detecting the string
	int findInc = 0;

	//Check if toFind is longer than string - if it is, it is physically impossible for it to exist
	if (tofSize > strSize) {
		tooBig = true;
	}

	if (!tooBig) {

		//Loop through string
		for (int i = 0; (i < strSize) && (!isIn); i++) {

			//Compare the string to toFind, increment findInc if loop has not successfully gone through the full toFind string
			if ((string[i] == toFind[findInc]) && (findInc != tofSize)) {
				findInc++;
			}
			else {
				findInc = 0;
			}

			//This only happens when the full toFind string has been incremented through, and thus it exists in the string
			if (findInc == tofSize) {
				isIn = true;
			}

		}

	}

	return isIn;

}

//Function repeatedMultiplication - Converts a number in some number base to base 10
int repeatedMultiplication(char* num, int base, char* table) {

	//Variables
	int numLen = strlen(num);
	int terminalN = 0;

	//Convert num from char to array of integer representations
	int* decReps = (int*)malloc(numLen * sizeof(int));	//Allocate space for an array same length as number of digits
	bool breakOut = false;								//Stop looking through the table when a match is found
	for (int i = 0; i < numLen; i++) {

		//Reset each time so that the loop below doesn't skip
		breakOut = false;

		//Look through the table, piece by piece. Is the value there? If so, add it's decimal value (equal to the index) to the array
		for (int j = 0; (j < 64 && !breakOut); j++) {

			if (!(num[i] == NULL)) {
				if (num[i] == table[j]) {
					decReps[i] = j;
					breakOut = true;
				}
			}
			
		}

	}

	//Begin mutliplying
	if (numLen == 1) {			//Special case: If only one number
		terminalN = decReps[0];
	}
	for (int i = 0; i < (numLen - 1); i++) {

		//Special initialization for first loop
		if (i == 0) {
			terminalN = ((decReps[i] * base) + decReps[i + 1]);
		}
		else {
			terminalN = ((terminalN * base) + decReps[i + 1]);
		}

	}

	//Free memory
	free(decReps);

	int finalDec = terminalN;
	return finalDec;
}

//Function repeatedDivision - Converts a number from base 10 to some number base
void repeatedDivision(char* num, int base, char* table, char* output) {
	
	//Variables
	int counter = 0;
	bool notZero = true;

	//Convert the number to an integer. Can safely just use simplerStrtol as num should always be base 10 in RD
	int intRep = simplerStrtol(num);
	int resultD = intRep; //Save it
	int resultS = 0;

	//Divide until result of division is 0
	for (int i = 0; notZero; i++) {

		//Divide
		if (base != 0) {
			resultD = intRep / base;
		}
		
		//Subtract original by the result
		resultS = intRep - (base * resultD);

		//Set next part of output to the character representation of resultS
		output[i] = table[resultS];

		//Set intRep to the result for next loop
		intRep = resultD;

		//Increment counter
		counter++;

		//Break the loop if the result of division was 0
		if (resultD == 0) {
			notZero = false;
		}
	}

	//Null-terminate the output string
	output[counter] = '\0';

	//Flip the output to make it correct
	strmrr(output, "", 0);

}

//Function simplerStrtol - Aims to simplify strtol to be usable in just one line while still being secure.
//Returns an integer representation of string if valid, 0 otherwise.
int simplerStrtol(char* string) {

	int returnVal = 0;		//Will be returned by function
	bool isValid = false;	//Is string a number
	char* end;				//When strtol stops

	//Try to convert
	int test = strtol(string, &end, 10);

	//Test
	if (string == end) {
		isValid = false;
	}
	else if (*end != '\0') {
		isValid = false;
	}
	else {
		isValid = true;
	}

	//Set if valid
	if (isValid) {
		returnVal = test;
	}

	return returnVal;
}

/*
*Function strmrr - "String Mirror" takes a string and mirrors it.
*string - the original string
*mirrored - the same string mirrored
*copy - if 0, changes the original string to be mirrored, if 1, copies into mirror
*if copy is 0, mirrored can be passed in as a string literal, as it won't be used
*if copy is 1, mirrored can be passed in as a string literal.
*mirrored CANNOT be a string literal if copy is 1 otherwise will be thrown an error
*string CANNOT be a string literal if copy is 0 otherwise will be thrown an error 
*/
void strmrr(char* string, char* mirrored, unsigned int copy) {

	//Get the lengths of strings
	int origLen = strlen(string);
	int mirrLen = strlen(mirrored);

	//Create a terminal string in case is not copying
	char* terminalString = (char*)malloc(origLen * sizeof(char));

	//Check for incorrect copy usage
	if (copy > 1) {
		puts("Error in strmrr, copy is not 1 or 0");
	}

	//Work for if copy
	else if (copy) {

		//Check that the sizes are correct
		if (origLen <= mirrLen) {

			//If everything checks out enough to begin, loop through the string backwards
			int counter = 0;
			for (int i = (origLen - 1); i >= 0; i--) {
				mirrored[counter] = string[i];
				counter++;
			}
			mirrored[counter] = '\0';

		}
		else {
			puts("Error in strmrr, mirrored string is not big enough");
		}

	}

	//Work for if not copy
	else if (!copy) {

		//Create the mirrored string
		int counter = 0;
		for (int i = (origLen - 1); i >= 0; i--) {
			terminalString[counter] = string[i];
			counter++;
		}
		terminalString[counter] = '\0';

		counter = 0;
		//Paste the mirrored string into original string
		for (int i = 0; i < origLen; i++) {
			string[i] = terminalString[i];
			counter++;
		}
		string[counter] = '\0';

	}

}

//Function openFileW - Opens a file for writing - Expects filename to include extension
//Potential addition in future - telling if it includes an extension and automatically appending .txt if there isn't
FILE* openFileW(char* filename) {

	//Create pointer to file
	FILE* file = fopen(filename, "w");

	if (file == NULL) {
		printf("Error in opening file for writing: FILE %s is null", filename);
	}

	return file;

}