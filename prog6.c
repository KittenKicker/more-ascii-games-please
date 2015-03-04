/***********************************************/
/* Devin Holland                                             
/* Login ID: holl4332                               
/* CS-202, Winter 2015                          
/* Programming Assignment 6                   
/* Adds/Appends user input to files and creates if needed                    
/***********************************************/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

//buffer for user input 1024 for reasonable length
char userInputBuffer[1024];

/***********************************************/
/* Function: getUserInput                                            
/* Purpose: read user input from standard in                               
/* Parameters: None                          
/* Returns: int                   
/***********************************************/
int getUserInput()
{
    read(0, userInputBuffer, 1024);

	return 0;
}

/***********************************************/
/* Function: Main                                             
/* Purpose: take in user input and handle calls to print
			to standard out and files.                               
/* Parameters: None                          
/* Returns: int                   
/***********************************************/
int main(int argc, char** argv)
{
	int option; //option currently being checked
	int inputFilesIndex = 0;
	int appendFlag = 0; //-a option passed in
	int ignoreFlag = 0; //-i option passed in
	char* argumentFile; //name of file in arguments
	FILE *inputFile; //file pointer for file

	//parse out the options
	while((option = getopt(argc, argv, "ai")) != -1)
	{
		switch(option)
		{
			//-a
			case 'a':
				appendFlag = 1;
				break;
			//-i
			case 'i':
				ignoreFlag = 1;
				break;
			//somemthing else
			default:
				perror("Invalid options");
				exit(1);
		}
	}

	//ignore sigint if -i
	if(ignoreFlag)
	{
		signal(SIGINT, SIG_IGN);
	}

	//if -a is not passed in
	if(appendFlag == 0)
	{
		//clear the files
		for(inputFilesIndex = optind; inputFilesIndex < argc; inputFilesIndex++)
		{
			argumentFile = argv[inputFilesIndex];

			//open in write mode
			inputFile = fopen(argumentFile, "w");

			//write nothing
			fprintf(inputFile,"%s", userInputBuffer);
			fclose(inputFile);
		}
	}

	//loop till a kill signal
	while(1)
	{
		//grab input from stdin
		getUserInput();

		//print gathered info to std out
		printf("%s", userInputBuffer);

		//loop through the input files
		for(inputFilesIndex = optind; inputFilesIndex < argc; inputFilesIndex++)
		{
			//get file name
			argumentFile = argv[inputFilesIndex];

			//open & write in append mode since we already cleared if needed
			inputFile = fopen(argumentFile, "a");
			fprintf(inputFile, "%s", userInputBuffer);
			fclose(inputFile);

		}

		//clear all of the memory from the buffer
		memset(userInputBuffer, 0, 1024);
	}
}