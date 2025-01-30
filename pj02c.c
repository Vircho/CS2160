//pj02c.c
//Author: Joshua White
//Class: CS2160
//Due Date: 11/17/24

//Libraries
#include <stdio.h> //Adds variety of functions
#include <stdbool.h> //Adds boolean functions
#include <string.h> //Adds string functions
#include <stdint.h> //Adds more int stuff like uint32_t
#include <math.h> //Adds math functions
#include <string.h> //Adds string functions

//Constants
#define MAX_STRING_LENGTH 256

//Function Definitions

//File functions
FILE* openFileR(char* filename);
FILE* openFileRB(char* filename);
FILE* openFileWB(char* filename);

//Analysis Functions
void utf8Analysis(FILE* utf8);
void intBinAnalysis(FILE* intb);

//Conversion Functions
int binToDec(char* bin);
void decToBin(char* dec, char* bin, int bits);
uint32_t betole(uint32_t beN);
void binToUTF(char* bin, char* utf8);;

//Find values
int leadingOnesBin(char* bin);
int leadingZerosBin(char* bin);;
void getTrueValUTF8(char* byte, char* value);

//String Functions
void trimStringFront(char* string);
void strmrr(char* string, char* mirrored, unsigned int copy);

//Other
void fillBytes(uint8_t bytes[], char* binary);

//Function main - main logic
int main(int argc, char* argv[]) {

	//Create two file pointers to the two files being used
	FILE* utf8 = openFileRB("UTF-8-demo.html");
	FILE* intb = openFileRB("integers.bin");

	//Analyze the utf8 file
	utf8Analysis(utf8);

	//Analyze the integer file
	intBinAnalysis(intb);

	//Close both files
	fclose(utf8);
	fclose(intb);
}

//Function utf8Analysis - analyzes a UTF-8 encoded file for several statistics.
void utf8Analysis(FILE* utf8) {

	//Variables
	//Will be printed
	unsigned int byteCount = 0;
	unsigned int asciiCount = 0;
	unsigned int totalVals = 0;
	unsigned int sum = 0;
	unsigned int leadings[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	//Tools
	unsigned int toSkip = 0; //Used to keep track of number of bytes past another
	char thisValue[64];

	//Move through the full file
	uint8_t thisByte = 0;
	while (fread(&thisByte, 1, 1, utf8)) {

		//Convert byte to binary
		char byteDec[4];
		char byteBin[9];
		sprintf(byteDec, "%d", thisByte);
		decToBin(byteDec, byteBin, 8);

		//Test for ascii
		if (byteBin[0] == '0') {
			asciiCount++;
			leadings[0]++;
		}

		//Find amount of leading ones
		int oneCount = leadingOnesBin(byteBin);
		
		//Increment based on amount
		switch (oneCount) {
		case 1:
			leadings[1]++;
			break;
		case 2:
			leadings[2]++;
			break;
		case 3:
			leadings[3]++;
			break;
		case 4:
			leadings[4]++;
			break;
		case 5:
			leadings[5]++;
			break;
		case 6:
			leadings[6]++;
			break;
		case 7:
			leadings[7]++;
			break;
		}

		//Find the value, and know how many to skip before getting another value

		if (toSkip == 0) {

			if ((oneCount == 1) || (oneCount == 0)) {
				toSkip = 0;
			}
			else {
				toSkip = oneCount;
			}
		}


		if ((oneCount > 0) && toSkip != 0) {
			getTrueValUTF8(byteBin, thisValue);
			toSkip--;

			//If toSkip has become 0 at this point, then all bytes for this value have been read
			if (toSkip == 0) {

				//Increment the sum
				int thisValueInt = binToDec(thisValue);
				sum += thisValueInt;

				//Increment total values
				totalVals++;
			}
		}
		else {
			
			//Find the value of the ascii number and increment the sum
			int thisValueInt = binToDec(byteBin);
			sum += thisValueInt;

			//Increment total values
			totalVals++;
		}

		//Increment byte count for total bytes regardless
		byteCount++;
	}

	//Final report
	printf("File size:...........\t%d bytes\n", byteCount);
	printf("ASCII codes:.........\t%d codes\n", asciiCount);
	printf("Number of values:....\t%d\n", totalVals);
	printf("Sum of values:.......\t%d\n", sum);
	printf("leading 1s: 0........\t%d\n", leadings[0]);
	printf("leading 1s: 1........\t%d\n", leadings[1]);
	printf("leading 1s: 2........\t%d\n", leadings[2]);
	printf("leading 1s: 3........\t%d\n", leadings[3]);
	printf("leading 1s: 4........\t%d\n", leadings[4]);
	printf("leading 1s: 5........\t%d\n", leadings[5]);
	printf("leading 1s: 6........\t%d\n", leadings[6]);
	printf("leading 1s: 7........\t%d\n", leadings[7]);
}

//Function intBinAnalysis - transforms a binary files into a utf-8 file then does analysis on it
void intBinAnalysis(FILE* intb) {

	//Open a new file to write to
	FILE* utf8F = openFileWB("integers.utf8");

	uint32_t thisByte = 0;
	while (fread(&thisByte, 4, 1, intb)) {

		//Variables
		char byteDec[11];
		char byteBin[33];
		char utf8[65] = { 0 };
		int neededBytes = 0;

		//Potential utf-8 values
		uint8_t utf8Bytes[7] = { 0 };

		//Convert to little endian
		thisByte = betole(thisByte);

		//Convert bytes to binary
		sprintf(byteDec, "%u", thisByte);
		decToBin(byteDec, byteBin, 32);

		//Find leading 0's
		int leadingZ = leadingZerosBin(byteBin);

		//Remove all leading 0's
		//Goes until there are no more leading 0's or until it's only one byte
		for (int i = 0; (i < leadingZ) && (strlen(byteBin) > 8); i++) {
			trimStringFront(byteBin);
		}

		//Add on UTF-8 Values
		binToUTF(byteBin, utf8);

		//Find amount of bytes needed to hold UTF-8 value
		neededBytes = leadingOnesBin(utf8);

		//Put into array
		fillBytes(utf8Bytes, utf8);

		//Print to file
		for (int i = 0; i < neededBytes; i++) {
			fwrite(&utf8Bytes[i], 1, 1, utf8F);
		}
	}

	//Run a UTF-8 Analysis on the file
	utf8Analysis(utf8F);

	//Close file
	fclose(utf8F);

}

//Function fillBytes - Fills an array representing bytes with a multi-byte binary number represented as a string
void fillBytes(uint8_t bytes[], char* binary) {

	int size = strlen(binary);
	int counter = 0;
	char thisByte[9];
	int nextToFill = 0;

	for (int i = 0; i < size; i++) {

		thisByte[counter] = binary[i];
		counter++;

		//When a byte has been filled
		if (counter == 8) {
			thisByte[counter] = '\0';
			bytes[nextToFill] = binToDec(thisByte);
			counter = 0;
			nextToFill++;
		}

	}


}

//Encode a binary integer in UTF-8
void binToUTF(char* bin, char* utf8) {

	//Variables
	int size = strlen(bin);
	bool ascii = false;
	int counter = 0;
	int nextIndex = 0;
	int bytes = 0;

	//Don't change ASCII values
	if (size == 8 && bin[0] == 0) {
		ascii = true;
		strcpy(utf8, bin);
	}
	else {

		//Fill the utf8 string front to back starting with back of binary, adding a 01 (10 backwards) every 6 bits
		for (int i = (size - 1); i >= 0; i--) {

			utf8[nextIndex] = bin[i];
			counter++;
			nextIndex++;

			if (counter == 6) {
				utf8[nextIndex] = '0';
				nextIndex++;
				utf8[nextIndex] = '1';
				nextIndex++;
				bytes++;
				counter = 0;
			}

		}

		//Deal with if next byte isn't a perfect size to be filled immediately
		while (!(8 - (bytes + 2) == counter)) {
			
			//Deal with if need too few bits to set leading 1's and have it be size of byte
			if (counter < (8 - (bytes + 2))) {
				utf8[nextIndex] = '0';
				nextIndex++;
				counter++;
			}

			//Deal with if too many bits to set leading 1's and have it be size of byte
			else {

				while (counter < 6) {
					utf8[nextIndex] = '0';
					nextIndex++;
					counter++;
				}
				utf8[nextIndex] = '0';
				nextIndex++;
				utf8[nextIndex] = '1';
				nextIndex++;
				bytes++;
				counter = 0;

				for (int i = 0; i < (8 - (bytes + 2)); i++) {
					utf8[nextIndex] = '0';
					nextIndex++;
					counter++;
				}
			}

		}

		//Deal with perfectly sized
		if (8 - (bytes + 2) == counter) {
			utf8[nextIndex] = '0';
			nextIndex++;
			for (int i = 0; i <= bytes; i++) {
				utf8[nextIndex] = '1';
				nextIndex++;
			}
			utf8[nextIndex] = '\0';
			strmrr(utf8, "", 0);
		}
	}

}

//Function decToBin - takes a string representing a decimal number and the amount of bits to store it
//and converts / stores as binary number in string.
//If amount of bits too low for decimal number, the binary digits get truncated.
void decToBin(char* dec, char* bin, int bits) {

	//Find amount of places
	long long place = bits - 1;
	place = pow(2, place);

	//Variables
	int binInc = 0;

	//Convert the string to an int
	char* end;
	double doubleDec = strtod(dec, &end);
	long long intDec = (long long)doubleDec;

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

}

//Function binToDec - Converts a binary number represented as a string into an integer
int binToDec(char* bin) {

	//Variables
	int place = 1;
	int bits = strlen(bin);
	int decimalRep = 0;

	int lsb = bits - 1;

	for (int i = lsb; i >= 0; i--) {

		if (bin[i] == '1') {
			decimalRep += place;
		}

		place *= 2;
	}

	return decimalRep;

}

//Function leadingOnesBin - Finds the amount of leading 1's in a binary number
int leadingOnesBin(char* bin) {

	//Variables
	int leading = 0;
	bool foundZ = false;

	//Get length of string
	int digits = strlen(bin);

	if (digits > 0) {

		for (int i = 0; i < digits, foundZ == false; i++) {

			if (bin[i] == '1') {
				leading++;
			}
			else {
				foundZ = true;
			}
		}

	}
	
	return leading;
}

//Function leadingOnesBin - Finds the amount of leading 1's in a binary number
int leadingZerosBin(char* bin) {

	//Variables
	int leading = 0;
	bool foundZ = false;

	//Get length of string
	int digits = strlen(bin);

	if (digits > 0) {

		for (int i = 0; i < digits, foundZ == false; i++) {

			if (bin[i] == '0') {
				leading++;
			}
			else {
				foundZ = true;
			}
		}

	}

	return leading;
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

//Function openFileRB - opens a file with a string of the file path for reading in binary
//Returns an error if cannot open file
FILE* openFileRB(char* filename) {

	//Create pointer to file
	FILE* file;

	//Attempt to open the file
	file = fopen(filename, "rb");

	if (file == NULL) {
		puts("Error in opening file for reading in binary - file is NULL");
	}

	return file;
}

//Removes the 11..0 from one byte of a UTF-8 encoded value
void getTrueValUTF8(char* byte, char* value) {

	char terminalString[9];
	bool foundZ = false;
	int counter = 0;

	//Find amount of leading 1's. This will decide whether this is the start of the utf-8 value or not
	int leading = leadingOnesBin(byte);

	if (strlen(byte) > 0) {

		for (int i = 0; i < strlen(byte); i++) {
			
			if (foundZ) {
				terminalString[counter] = byte[i];
				counter++;
			}
			
			//By doing this if statement after the first, I skip the first 0.
			if (byte[i] == '0') {
				foundZ = true;
			}
		}

		terminalString[counter] = '\0';

		if (leading > 1) {
			strcpy(value, terminalString);
		}
		else {
			strcat(value, terminalString);
		}
	}
}

//Function betole - converts an unsigned 32 bit big endian number to an unsigned 32 bit little endian number
uint32_t betole(uint32_t beN) {

	//Create value to represent the little endian version of the binary number
	uint32_t leNB1;
	uint32_t leNB2;
	uint32_t leNB3;
	uint32_t leNB4;

	//Use bit-shifting to convert the big endian number to little endian
	//Shift each byte to their positon Byte1 -> Position 4, Byte2 -> Position 3, etc.
	//&0x00 etc: These make a new binary number which is all 0's except for the byte that's being worked on.
	//The | then combines them all
	leNB1 = ((beN >> 24) & 0x000000FF);
	leNB2 = ((beN >> 8) & 0x0000FF00);
	leNB3 = ((beN << 8) & 0x00FF0000);
	leNB4 = ((beN << 24) & 0xFF000000);

	uint32_t leN = leNB1 | leNB2 | leNB3 | leNB4;

	return leN;
}

//Function openFileW - Opens a file for writing in binary - Expects filename to include extension
//Potential addition in future - telling if it includes an extension and automatically appending .bin if there isn't
FILE* openFileWB(char* filename) {

	//Create pointer to file
	FILE* file = fopen(filename, "wb");

	if (file == NULL) {
		printf("Error in opening file for writing: FILE %s is null", filename);
	}

	return file;

}

//Function trimStrFront - trims the front of the string by removing string[0]
void trimStringFront(char* string) {

	//Size of string
	int strSize = strlen(string);

	//Make copy of stirng
	char copy[MAX_STRING_LENGTH];

	//Move through the string and copy characters in starting with string[1]
	int copyInc = 0;
	for (int i = 0; i < strSize; i++) {

		if (i != 0) {
			copy[copyInc] = string[i];
			copyInc++;
		}
	}

	//Null-terminate the copy
	copy[copyInc] = '\0';

	//Find size of the copy
	int copySize = strlen(copy);

	//Copy the copy back into the original string
	int strInc = 0;
	for (int i = 0; i < copySize; i++) {
		string[i] = copy[i];
		strInc++;
	}

	//Null-terminate to cut off the original string
	string[strInc] = '\0';

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