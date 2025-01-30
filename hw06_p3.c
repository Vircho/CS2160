//hw06_p3.c
//Author: Joshua White
//Class: CS2160
//Due Date: 10/20/24

//Libraries
#include <stdio.h> //Adds variety of functions
#include <stdbool.h> //Adds boolean functions
#include <dirent.h> //Adds directory/entry functions

//Constants

//Function Definitions
bool findChar(const char* string, char toFind);
int countFileLines(char* filename);
FILE* openFileR(char* filename);
void findVMPrintLinecount(char* folderName);
bool findString(const char* string, const char* toFind);

//Function main - main logic
int main(int argc, char* argv[]) {

	//Check for argument
	if (argv[1] == NULL) {
		puts("Error: No argument given");
	}
	else {

		//Check for file
		if (findChar(argv[1], '.')) {

			//If file, count file lines & print
			int fileLines = countFileLines(argv[1]);
			printf("Argument is a file - Number of lines: %d\n", fileLines);

		}
		else {

			//If folder find all .vm files & count those files's linecount
			findVMPrintLinecount(argv[1]);

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
		puts("Error in findString: toFind is longer than string");
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

//Function countFileLines - returns an int representing number of lines in file (does not include blank lines)
//Other functions required: openFileR
int countFileLines(char* filename) {

	//Variables
	//Amount of lines in the file
	int lines = 0;
	//The character currently pointed at in the file
	char thisChar;
	//bool saying if the line is completely blank
	bool isBlank = true;

	//Open file to count lines in
	FILE* file = openFileR(filename);

	//Loop through the file
	while ((thisChar = fgetc(file)) != EOF) {

		//Detect any characters that are not space, tab, or newline
		if ( (thisChar != ' ') && (thisChar != '\t') && (thisChar != '\n') ) {
			isBlank = false;
		}

		//Increment line count upon encountering a newline character
		if ( (thisChar == '\n') && !isBlank) {
			lines++;
			isBlank = true; //Reset isBlank for next line
		}
	}

	//Since the loop does not count the final line
	//due to the final line of text files not having a newline, 
	//manually increment one more line to represent final line
	//Addition as of 10/16/24 checks if final line was blank first
	if (!isBlank) {
		lines++;
	}

	//Close the file when done
	fclose(file);

	return lines;
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

//Function findVMPrintLinecount - given name of a folder on user system, finds all .vm files and prints the amount of lines in each one
//Functions required: countFileLines
void findVMPrintLinecount(char* folderName) {

	//Create pointer to the folder and a struct to represent the items
	DIR* folder = opendir(folderName);
	struct dirent* item;

	//How many lines are in a .vm file
	int lineNum = 0;

	//Loop through all items in the folder
	while ((item = readdir(folder)) != NULL) {
		
		//Look at the name of the item and see if it's a .vm file
		if ((findString(item->d_name, ".vm")) ) {

			//Count number of lines
			lineNum = countFileLines(item->d_name);
			printf(".vm file found: %s | Number of lines: %d", item->d_name, lineNum);
		}
	}

	//Close the folder
	closedir(folder);

}