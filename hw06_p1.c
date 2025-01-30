//hw06_p1.c
//Author: Joshua White
//Class: CS2160
//Due Date: 10/20/24

//Libraries
#include <stdbool.h> //adds boolean functions
#include <string.h> //adds string functions
#include <stdio.h> //adds variety of functions

//Constants
#define MAX_STRING_LENGTH 256

//Function Definitions
void print2DCharArr_DirFl(char** arr, int rows);
bool findChar(const char* string, char toFind);
void splitStringAt(const char* originalString, char* newString0, char* newString1, char splitIndex);
void trimStringFront(char* string);

//int main - main logic
int main(int argc, char* argv[]) {

	//Check that all arguments are given
	if (argv[3] == NULL) {
		puts("Error: Not enough arguments. Must have 3 arguments");
	}
	else {

		//Print the amount of arguments given to the code
		int argN = argc - 1; //argc includes the call to the program, so -1 to remove that

		printf("Amount of arguments given: %d\n", argN);

		//Print all the arguments given
		print2DCharArr_DirFl(argv, argc);
	}

}

//Function print2DCharArr_DirFl - prints a 2d character array as directories and files
//Takes in an array of strings and the number of rows.
void print2DCharArr_DirFl(char** arr, int rows) {

	//Start at line 1, run through the array for amount of rows and detect any periods.
	for (int i = 1; i < rows; i++) {

		//Find the size of the current string
		int sizeString = strlen(arr[i]);

		//Define boolean variables to say if the string is a dir or file
		bool isFil = false;

		//Detect for file
		//If the string contains a '.'
		if (findChar(arr[i], '.')) {
			isFil = true;
		}
		else {
			isFil = false;
		}

		if (isFil == true) {

			//Create new strings to hold the split file
			char newFile0[MAX_STRING_LENGTH];
			char newFile1[MAX_STRING_LENGTH];

			//Split the string into the two new strings
			splitStringAt(arr[i], newFile0, newFile1, '.');

			//Trim the second string, as splitStringAt keeps the splitIndex at the front of the second string
			trimStringFront(newFile1);

			//Print the strings with extra bits
			printf("<FILE> [%s].[%s]\n", newFile0, newFile1);

		}
		else {

			//If it's not a file, the string can just be printed
			printf("<DIR> %s\n", arr[i]);
		}

	}

}

//Function findChar - takes in a string and a character to find - returns true if the char exists in the string
bool findChar(const char* string, char toFind) {

	//Variable to return
	bool isIn = false;

	//Find the length of the string
	int stringSize = strlen(string);

	//Run through the string character by character until the string either ends or the character is found
	for (int i = 0; i < stringSize && !isIn; i++) {

		if (string[i] == toFind) {
			isIn = true;
		}
	}

	return isIn;
}

//Function splitString at - takes 3 strings, a string to split, two strings to put them in, and a char to split them at.
//Note: the char that the strings get split at goes into newString1[0]
void splitStringAt(const char* originalString, char* newString0, char* newString1, char splitIndex) {

	//The length of the string to be split
	int orLen = strlen(originalString);

	//The lengths of the strings that hold the original split
	int ns0Len = strlen(newString0);
	int ns1Len = strlen(newString1);

	//Check to make sure that the splitIndex exists inside the string
	bool splitIndexExists = findChar(originalString, splitIndex);
	if (!splitIndexExists) {
		puts("Error in splitStringAt: splitIndex is not inside the original string");
	}

	//If the two strings aren't collectively long enough to hold the original, throw an error
	//<= is used because one extra null-terminator will be added
	bool stringsRightSize = true;
	if ((ns0Len + ns1Len) <= orLen) {
		puts("Error in splitStringAt: New strings aren't big enough to hold the original string");
		stringsRightSize = false;
	}

	//Begin splitting the strings
	bool strInHit = false;
	int newStInc0 = 0;
	int newStInc1 = 0;
	for (int i = 0; i < orLen; i++) {

		if (originalString[i] == splitIndex) {
			strInHit = true;
		}


		if (!strInHit) {
			newString0[newStInc0] = originalString[i];
			newStInc0++;
		}
		else if (strInHit) {
			newString1[newStInc1] = originalString[i];
			newStInc1++;
		}

	}

	//Null-terminate both new strings
	newString0[newStInc0] = '\0';
	newString1[newStInc1] = '\0';

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