//hw06_p2.c
//Author: Joshua White
//Class: CS2160
//Due Date: 10/20/24

//Libraries
#include <stdio.h> //Adds variety of functions
#include <dirent.h> //Adds ability to handle directories

//Constants

//Function Definitions

//Function main - main logic
int main(int argc, char* argv[]) {

	//Create the directory the program is excecuted from
	DIR* exFrom = opendir(".");

	//Create a directory entry (stuff that exists in the directory: folders, files, etc)
	struct dirent* item;

	//Loop and name each item
	//readdir reads each item in the directory one by one (i go until it reaches NULL)
	//item is a struct, and d_name is the name of the item.
	while ((item = readdir(exFrom)) != NULL) {
		printf("Name: %s\n", item->d_name);
	}

	//Close the directory
	closedir(exFrom);

}