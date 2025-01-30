//hw06_p4.c
//Author: Joshua White
//Class: CS2160
//Due Date: 10/20/24

//Libraries
#include <stdio.h> //Adds variety of functions
#include <dirent.h> //Adds ability to handle directories
#include <stdbool.h> //Adds boolean functions
#include <string.h> //Adds string functions
#include <stdint.h> //Adds more int stuff like uint32_t

//Constants
#define STRING_LENGTH 256

//Function Definitions

//Functions that do things with strings
bool findString(const char* string, const char* toFind);
void allocate2DIntArray(int** intArr, int arrSize, int rowSize);
void removeFileExtension(const char* filename, char* extensionlessName);
void topLinetoString(char* filename, char* toPut);

//Functions that do things with directories
int entInDir(DIR* folder);
void createSets(char* foldername, int itemCount);

//File functions
FILE* openFileR(char* filename);
FILE* openFileRB(char* filename);

//Table functions
void fillIntArrCol(int** table, int rowCount, int fillWith, int columnNum);

//Other functions
uint32_t betole(uint32_t beN);

//Function main - main logic
int main(int argc, char* argv[]) {

	//Get argv[1] as a string
	char* foldername = argv[1];

	//Check the command line-argument
	if (argv[1] == NULL) {
		puts("Error: Command line argument missing");
	}
	else {

		//Open the directory
		DIR* folder = opendir(argv[1]);

		//Check that the directory exists
		if (folder == NULL) {
			puts("Error: Entered folder does not exist");
		}
		else {

			//Find number of files in folder
			int itemCount = entInDir(folder);

			//Close the directory
			closedir(folder);

			//Create the sets
			createSets(foldername, itemCount);

		}

	}

}

//Function entInDir - Finds number of entries in a folder minus the periods at the start
int entInDir(DIR* folder) {

	//Variables
	int num = 0;

	//Test if folder is NULL
	if (folder == NULL) {
		puts("Error in entInDir: Directory is NULL");
	}
	else {

		struct dirent* item;

		//Loop through the folder
		while ((item = readdir(folder)) != NULL) {
			num++;
		}

		//Subtract 2 from the num to get rid of the root
		num = num - 2;
	}

	return num;
}

//Function createSets - taking the name of a folder, and the amount of items in it, organizes them by name & content
void createSets(char* foldername, int itemCount) {

	//Allocate enough memory for a string array that holds all used files
	int** table = (int**)malloc(itemCount * sizeof(int*));	

	//Fill the string array with something other than NULL, as I do not want to deal with that
	allocate2DIntArray(table, itemCount, 4);

	//Fill each part of the table with -1 to keep a constant "if it's this, it hasn't been filled" value
	for (int i = 0; i < 4; i++) {
		fillIntArrCol(table, itemCount, -1, i);
	}

	//Variables
	char* filetype; //Which file is being looked for (.txt, .le, or .be)

	//Loop through the 3 types of files
	for (int i = 0; i < 3; i++) {

		//Variables
		int nextRow = 0;
		int nextColumn = 0;
		bool isFile = true;

		//Decide the filename
		switch (i) {
		case 0:
			filetype = ".txt";
			nextColumn = 1;
			break;
		case 1:
			filetype = ".le";
			nextColumn = 2;
			break;
		case 2:
			filetype = ".be";
			nextColumn = 3;
			break;
		}

		//Open the folder and initialize the dirent on each loop
		DIR* folder = opendir(foldername);
		struct dirent* item;

		//Go through the folder
		while ( (item = readdir(folder)) != NULL) {

			//Make sure is valid file by using strcmp on both root files
			if ( (strcmp(item->d_name, ".") == 0) || (strcmp(item->d_name, "..") == 0)) {
				isFile = false;
			}
			else {
				isFile = true;
			}

			//As long as it is a valid file
			if (isFile) {

				//As long as we're looking at the text files
				if ((strcmp(filetype, ".txt")) == 0) {

					//As long as it sees a text file
					if (findString(item->d_name, ".txt")) {

						//begin filling the first two columns of the array

						//Create the full file path, as it's needed to open the file
						char fullFilePath[STRING_LENGTH];
						strcpy(fullFilePath, foldername);
						strcat(fullFilePath, "/");
						strcat(fullFilePath, item->d_name);

						//Input the first line from the file
						char num[STRING_LENGTH];
						topLinetoString(fullFilePath, num);

						//Convert that line into an int
						char* end;
						double strtodOut = strtod(num, &end);
						int numInFile = (int)strtodOut;

						//Add the number to the array
						table[nextRow][0] = numInFile;

						//Remove the file extension from the file name
						char pureFilename[STRING_LENGTH];
						removeFileExtension(item->d_name, pureFilename);

						//Convert the pure file name to an int
						strtodOut = strtod(pureFilename, &end);
						int filenameNum = (int)strtodOut;

						//Add to the table
						table[nextRow][nextColumn] = filenameNum;

						//Increment the rows
						nextRow++;

					}

				}

				//As long as we're looking at the le files
				else if ((strcmp(filetype, ".le")) == 0) {

					//As long as it sees a le file
					if (findString(item->d_name, ".le")) {

						//Begin filling the column

						//Create the full file path, as it's needed to open the file
						char fullFilePath[STRING_LENGTH];
						strcpy(fullFilePath, foldername);
						strcat(fullFilePath, "/");
						strcat(fullFilePath, item->d_name);

						//Open the file and read in the number
						unsigned int binNum;
						FILE* leFile = openFileRB(fullFilePath);
						fread(&binNum, 4, 1, leFile);

						//Get the name of the file
						//Remove the file extension from the file name
						char pureFilename[STRING_LENGTH];
						removeFileExtension(item->d_name, pureFilename);

						//Convert the pure file name to an int
						char* end;
						double strtodOut = strtod(pureFilename, &end);
						int filenameNum = (int)strtodOut;

						//Check the number among all numbers in the numbers column
						bool found = false;
						for (int i = 0; (i < itemCount) && (!found); i++) {


							if ((binNum == table[nextRow][0]) && (table[nextRow][nextColumn] == -1)) {
								table[nextRow][nextColumn] = filenameNum;
								found = true;
							}
							else {
								nextRow++;
							}

						}

						//Reset nextRow after is is done looping
						nextRow = 0;

					}

				}
				else if ((strcmp(filetype, ".be")) == 0) {

					//As long as it sees a be file
					if (findString(item->d_name, ".be")) {

						//Begin filling the column

						//Create the full file path, as it's needed to open the file
						char fullFilePath[STRING_LENGTH];
						strcpy(fullFilePath, foldername);
						strcat(fullFilePath, "/");
						strcat(fullFilePath, item->d_name);

						//Open the file and read in the number
						uint32_t binNum;
						FILE* beFile = openFileRB(fullFilePath);
						fread(&binNum, 4, 1, beFile);

						//Turn the binNum into a little endian value
						uint32_t leVer = betole(binNum);
						unsigned int finalBinNum = (int)binNum;

						//Get the name of the file
						//Remove the file extension from the file name
						char pureFilename[STRING_LENGTH];
						removeFileExtension(item->d_name, pureFilename);

						//Convert the pure file name to an int
						char* end;
						double strtodOut = strtod(pureFilename, &end);
						int filenameNum = (int)strtodOut;

						//Check the number among all numbers in the numbers column
						bool found = false;
						for (int i = 0; (i < itemCount) && (!found); i++) {


							if ((finalBinNum == table[nextRow][0]) && (table[nextRow][nextColumn] == -1)) {
								table[nextRow][nextColumn] = filenameNum;
								found = true;
							}
							else {
								nextRow++;
							}

						}

						//Reset nextRow after is is done looping
						nextRow = 0;

					}
				}

			}


		}

		//Close the folder
		closedir(folder);

	}

	//Once the table has been filled, run through and find all rows where column 1,2,3 are not -1, then add the values in those spots and print as set
	int nextRow = 0;
	for (int i = 0; i < itemCount; i++) {

		//Check if the values in the table are not -1
		bool isSet = ((table[nextRow][1] != -1) && (table[nextRow][2] != -1) && (table[nextRow][3] != -1));

		if (isSet) {
			int setNum = (table[nextRow][1] + table[nextRow][2] + table[nextRow][3]);
			printf("Set #%d: %d\n", (nextRow + 1), setNum);
		}

		nextRow++;
	}

}

//Function allocate2DIntArray - Allocates enough memory for a 2D Int Array using the amount of rows (arrSize) and columns (rowSize) are needed
void allocate2DIntArray(int** intArr, int arrSize, int rowSize) {

	for (int i = 0; i < arrSize; i++) {
		intArr[i] = (int*)malloc(rowSize * sizeof(int));
	}

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

//Function topLinetoString - takes the top line of a file and inputs it into a string
void topLinetoString(char* filename, char* toPut) {

	//Open the file
	FILE* file = openFileR(filename);

	//Variables
	char thisChar = 'x';
	int stringInc = 0;
	bool eol = false;

	while ((thisChar = fgetc(file)) != EOF) {

		//Begin filling until reach newline (if file is only one line, the eof will still apply)
		if (thisChar == '\n') {
			eol = true;
		}

		if (!eol) {
			toPut[stringInc] = thisChar;
			stringInc++;
		}

	}

	//Null-terminate
	toPut[stringInc] = '\0';

	//Close File
	fclose(file);
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

//Function removeFileExtension - removes the file extension from a file name & returns a string
//representing the filename sans the extension
void removeFileExtension(const char* filename, char* extensionlessName) {

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

	//Add in the pure file name
	strcpy(extensionlessName, pureFlName);
}

//Function fillIntArrCol - Fills a column of a 2D int array with one number using the amount of rows, what to fill with, and which column to fill
void fillIntArrCol(int** table, int rowCount, int fillWith, int columnNum) {

	int row = 0;
	for (int i = 0; i < rowCount; i++) {
		table[row][columnNum] = fillWith;
		row++;
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