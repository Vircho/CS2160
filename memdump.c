//memdump.c
//Author: Joshua White
//Class: CS2160
//Due Date: 11/17/24


#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <memdump.h>

#define MATH_PI (acos(-1.0))
#define MATH_E  (exp(1.0))

//Function memgen - Takes in a pointer to an unsigned integer representing 
void* memgen(uintptr_t n)
{
	//Hex representation of a piece of memory?
	unsigned char* mem;

	//Create a new seed
	srand(time(NULL));

	//Allocate space for a piece of memory in the mem string?
	mem = (uint8_t*)malloc(n);

	//
	for (int i = 0; i < n; i++)
		mem[i] = rand() & 0xFF;

	//
	for (int i = 0; i < 16; i++)
		mem[rand() % n] = 0;

	//
	((double*)mem)[3] = MATH_PI;
	((float*)mem)[25] = MATH_PI;
	((double*)mem)[6] = MATH_E;
	((float*)mem)[1] = MATH_E;
	((int*)mem)[2] = 4242424242;
	((short*)mem)[19] = 12345;
	((short*)mem)[20] = -3333;

	//
	strcpy(mem + 59, "PJ02 Mem Gen.");
	strcpy((uint8_t*)(((double*)mem) + 13), "CS-2160");

	return (void*)mem;
}

//Function fmemdump - takes in a file-pointer (if it exists, read memory segments from it),
//A pointer base to the first address being looked at,
//the amount of bytes in the dump "size",
//A "bool" deciding whether to also print the ascii,
//A char to use when can't print the ascii.
void fmemdump(FILE* fp, void* base, uintptr_t size, int ascii, char placeholder) {

	//How is this function going to be used?
	//N = null with ascii
	//n = null without ascii
	//F = file with ascii
	//f = file without ascii
	//2 Super modes: Null = N or n | File = F or f
	char modeType = 'x';

	//Set the mode type
	if (fp == NULL) {
		if (ascii) {
			modeType = 'N';
		}
		else {
			modeType = 'n';
		}
	}
	else {
		if (ascii) {
			modeType = 'F';
		}
		else {
			modeType = 'f';
		}
	}

	//Super-Mode Null
	if ((modeType == 'N') || (modeType == 'n')) {

		//Print the top of the table based on mode
		if (modeType == 'N') {
			puts("+------------------+-------------------------------------------------+----------------+");
			puts("|     ADDRESS:     |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |0123456789ABCDEF|");
			puts("+------------------+-------------------------------------------------+----------------+");
		}
		else if (modeType == 'n') {
			puts("+------------------+-------------------------------------------------+");
			puts("|     ADDRESS:     |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |");
			puts("+------------------+-------------------------------------------------+");
		}

		//Cast the baseAdr_n as a uintptr_t
		uintptr_t baseAdrN = (uintptr_t)base;

		//Clone for another one with final nibble 0
		uintptr_t thisAdrIN = (uintptr_t)base;

		//Set the final nibble of the address to 0
		thisAdrIN = thisAdrIN & (0xFFFFFFFFFFFFFFF0);

		//Reset both as a pointer
		base = (void*)baseAdrN;
		void* thisAdrN = (void*)thisAdrIN;

		//Begin printing the table until we are going outside of the range
		int inRangeN = 1;
		//represents whether printing anything or not.
		//updated to 1 when thisAdrI_n = base
		//updated to 0 when amountPrinted = size
		int printingN = 0;
		int amountPrintedN = 0;

		//16 char array - used to print all ascii values
		char asciiArrN[17];

		while (inRangeN) {

			//Start each loop with the address
			//Extra space afterwards for formatting
			printf("| %p | ", thisAdrN);

			//A for loop that goes for 16 address locations
			for (int i = 0; i < 16; i++) {

				//Tests for printing
				if ((thisAdrN == base) && (printingN != 1)) {
					printingN = 1;
				}
				else if (amountPrintedN == size) {
					printingN = 0;
				}

				//Begin printing
				if ((printingN) && (amountPrintedN != size)) {

					//Start by printing the value in that location as a hex value
					//Cast the address
					//The reason it's being casted to a char pointer instead of any other data type is 2 reasons:
					//1. the size of a char guarentees 2 hex digits, which is the amount desired.
					//2. this allows for easy ascii usage.
					char* thisAdrNC = (char*)thisAdrN;

					//Save as a char - by being an unsigned char, it is guarenteed to be only 1 byte, the amount being printed
					unsigned char currentV = *thisAdrNC;

					//Print as hex
					//Explanation to parts:
					//% - specify format
					//02 - two digits only (despite only being 1 byte, there's no guarentee it won't be <2 digits without this)
					//X in capital hexadecimal
					printf("%02X ", currentV);

					//Add the ascii equivilent to the array (by casting it to a char, i've already done so)
					//Test if printable
					if (isprint(currentV)) {
						asciiArrN[i] = currentV;
					}
					else {

						//Check if placeholder can be printed
						if (isprint(placeholder)) {
							asciiArrN[i] = placeholder;
						}
						else {
							//Else use a period
							asciiArrN[i] = '.';
						}
					}

					//Increment amount printed
					amountPrintedN++;

				}
				else {

					//Print nothing
					printf("    ");

					//Add a space to the ascii array
					asciiArrN[i] = ' ';

				}

				//Increment the pointer
				//Reason for char pointer - incrementing by one byte, and an unsigned char is exactly one byte
				unsigned char* terminalP = (unsigned char*)thisAdrN; //Casting it before doing arithmetic
				terminalP++;
				thisAdrN = (void*)terminalP;

			} //End of 16-address for loop

			//After the for-loop, print the ascii based on mode
			if (modeType == 'N') {
				asciiArrN[16] = '\0';
				printf("|%s|\n", asciiArrN);
			}
			else if (modeType == 'n') {
				puts("|");
			}

			//Check if continuing printing to see if going for another loop
			if (amountPrintedN == size) {
				inRangeN = 0;
			}

		}//End of while loop

		//After all memory locations have been printed, end the table
		if (modeType == 'N') {
			puts("+------------------+-------------------------------------------------+----------------+");
		}
		else if (modeType == 'n') {
			puts("+------------------+-------------------------------------------------+");
		}

	}//End of super mode Null

	//Super-Mode File
	if ((modeType == 'F') || (modeType == 'f')) {

		//Variables
		int counter = 0;

		//Print the top of the table based on mode
		if (modeType == 'F') {
			puts("+------------------+-------------------------------------------------+----------------+");
			puts("|     ADDRESS:     |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |0123456789ABCDEF|");
			puts("+------------------+-------------------------------------------------+----------------+");
		}
		else if (modeType == 'f') {
			puts("+------------------+-------------------------------------------------+");
			puts("|     ADDRESS:     |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |");
			puts("+------------------+-------------------------------------------------+");
		}

		//Cast the baseAdr_n as a uintptr_t
		uintptr_t baseAdrF = (uintptr_t)base;

		//Clone for another one with final nibble 0
		uintptr_t thisAdrIF = (uintptr_t)base;

		//Set the final nibble of the address to 0
		thisAdrIF = thisAdrIF & (0xFFFFFFFFFFFFFFF0);

		//Reset both as a pointer
		base = (void*)baseAdrF;
		void* thisAdrF = (void*)thisAdrIF;

		//Begin printing the table until we are going outside of the range
		int inRangeF = 1;
		//represents whether printing anything or not.
		//updated to 1 when thisAdrI_n = base
		//updated to 0 when amountPrinted = size
		int printingF = 0;
		int amountPrintedF = 0;

		//16 char array - used to print all ascii values
		char asciiArrF[17];

		//File addition - value to store the byte
		uint8_t thisByte = 0;

		while ((fread(&thisByte, 1, 1, fp) == 1) && (inRangeF)) {

			if (counter == 0) {

				//Start each loop with the address
				//Extra space afterwards for formatting
				printf("| %p | ", thisAdrF);

			}

			//Tests for printing
			if ((thisAdrF == base) && (printingF != 1)) {
				printingF = 1;
			}
			else if (amountPrintedF == size) {
				printingF = 0;
			}

			//Begin printing
			if ((printingF) && (amountPrintedF != size)) {

				//Print as hex
				//Explanation to parts:
				//% - specify format
				//02 - two digits only (despite only being 1 byte, there's no guarentee it won't be <2 digits without this)
				//X in capital hexadecimal
				printf("%02X ", thisByte);

				//Add the ascii equivilent to the array (by casting it to a char, i've already done so)
				//Test if printable
				if (isprint(thisByte)) {
					asciiArrF[counter] = thisByte;
				}
				else {

					//Check if placeholder can be printed
					if (isprint(placeholder)) {
						asciiArrF[counter] = placeholder;
					}
					else {
						//Else use a period
						asciiArrF[counter] = '.';
					}
				}

				//Increment amount printed
				amountPrintedF++;

			}
			else {

				//Print nothing
				printf("   ");

				//Add a space to the ascii array
				asciiArrF[counter] = ' ';

			}

			//Increment the pointer
			//Reason for char pointer - incrementing by one byte, and an unsigned char is exactly one byte
			unsigned char* terminalP = (unsigned char*)thisAdrF; //Casting it before doing arithmetic
			terminalP++;
			thisAdrF = (void*)terminalP;

			//Increment counter
			counter++;

			//After the for-loop, print the ascii based on mode
			if (counter == 16) {
				if (modeType == 'F') {
					asciiArrF[16] = '\0';
					printf("|%s|\n", asciiArrF);
				}
				else if (modeType == 'f') {
					puts("|");
				}

				//Check if continuing printing to see if going for another loop
				if (amountPrintedF == size) {
					inRangeF = 0;
				}
				counter = 0;
			}


		}//End of while loop

		//After all memory locations have been printed, end the table
		if (modeType == 'F') {
			puts("+------------------+-------------------------------------------------+----------------+");
		}
		else if (modeType == 'f') {
			puts("+------------------+-------------------------------------------------+");
		}
	}
}