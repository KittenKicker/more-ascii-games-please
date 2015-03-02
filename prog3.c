/***********************************************/
/* Devin Holland                                             
/* Login ID: holl4332                               
/* CS-202, Winter 2015                          
/* Programming Assignment 3                         
/* Mimics subset of touch. -a -m -t -c                    
/***********************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <utime.h>

time_t convertedTime; //time converted to time_t
/***********************************************/
/* Function: checkForFile                                             
/* Purpose: check to see if file exists or not                               
/* Parameters: char* nameOfFile: name of file to check                          
/* Returns: int (true/false)                  
/***********************************************/
int checkForFile(char *nameOfFile);

/***********************************************/
/* Function: setTimes                                             
/* Purpose: takes the user input time and parses it if it can                               
/* Parameters: char* inputTime: user input time
			   int accessFlag: -a input or not
			   int modifyFlag: -m input or not
			   struct stat fileInfo: times currently set for file
			   char* currentFile: the name of the file to be updated
/* Returns: int (true/false)                  
/***********************************************/
int setTimes(char* inputTime);

/***********************************************/
/* Function: writeTimes                                             
/* Purpose: takes the user input time and parses it if it can                               
/* Parameters: time_t inputTime: user input time as a time_t
			   int accessFlag: -a input or not
			   int modifyFlag: -m input or not
			   struct stat fileInfo: times currently set for file
			   char* currentFile: the name of the file to be updated
/* Returns: int (true/false)                  
/***********************************************/
int writeTimes(time_t inputTime, struct stat fileInfo, int accessFlag, int modifyFlag, char* currentFile);

/***********************************************/
/* Function: touchOutput                                             
/* Purpose: output info about touch format                               
/* Parameters: none                          
/* Returns: int (true/false)                  
/***********************************************/
int touchOutput();

/***********************************************/
/* Function: main                                             
/* Purpose: parse through options and call correct time update methods                               
/* Parameters: int argc: number of arguments
			   char **argv: the list of arguments passed in                         
/* Returns: int (true/false)                  
/***********************************************/
int main(int argc, char **argv)
{
	struct stat fileInfo; //information about the current file

	char *inputTime; //user input for -t param
	int inputFilesIndex; //counter to loop through input files
	char *currentFile; //name of the current file

	int currentOption = 0; //which option are we looking at?
	int hasTimeOption = 0; //-t specified
	int hasAccessOnlyOption = 0; //-a specified
	int hasModifyOnlyOption = 0; //-m specified
	int hasNoCreateFlag = 0; //-c specified

	if(argc == 1) //you need at least 2 arguments otherwise you haven't input a file name
	{
		printf("Missing file operand\n");
		touchOutput();
		exit(1);
	}

	//use get ops to loop through input options
	while((currentOption = getopt(argc, argv, "t:amc")) != -1)
	{
		switch(currentOption)
		{
			case 't':
				hasTimeOption = 1; //set -t flag
				inputTime = optarg; //set time input
				break;
			case 'a':
				hasAccessOnlyOption = 1; //set -a flag
				break;
			case 'm':
				hasModifyOnlyOption = 1; //set -m flag
				break;
			case 'c':
				hasNoCreateFlag = 1; //set -c flag
				break;
			default: //input that isn't expected
				printf("Invalid Options\n");
				touchOutput();
				exit(1);
		}
	}

	//check to see if time is valid
	if(hasTimeOption)
	{
		if(setTimes(inputTime) == 0)
		{
			touchOutput(); //print layout and exit
			exit(1);
		}
	}

	//loop through the input files
	for(inputFilesIndex = optind; inputFilesIndex < argc; inputFilesIndex++)
	{
		currentFile = argv[inputFilesIndex]; //get name of file
		if(checkForFile(currentFile)) // if the file exists
		{
			stat(currentFile, &fileInfo); //grab info about file

			if(hasTimeOption) //if we got an input time use it
			{
				writeTimes(convertedTime, fileInfo, hasAccessOnlyOption, hasModifyOnlyOption, currentFile);	
			}
			else //use current time
			{
				writeTimes(time(NULL), fileInfo, hasAccessOnlyOption, hasModifyOnlyOption, currentFile);
			}
		}
		else //if file doesn't exist
		{
			if(hasNoCreateFlag != 1) //if -c isn't specified
			{
				fopen(currentFile, "ab+"); //create the file
				stat(currentFile, &fileInfo); //get the info about it

				if(hasTimeOption) //only need to update new file if we got a time input
				{
					writeTimes(convertedTime, fileInfo, hasAccessOnlyOption, hasModifyOnlyOption, currentFile);
				}
			}
		}
	}

	return 0;
}

/***********************************************/
/* Function: checkForFile                                             
/* Purpose: check to see if file exists or not                               
/* Parameters: char* nameOfFile: name of file to check                          
/* Returns: int (true/false)                  
/***********************************************/
int checkForFile(char *nameOfFile)
{
	struct stat fileInfo; //hold file info
	return (stat(nameOfFile, &fileInfo) == 0); //good if we get info back
}

/***********************************************/
/* Function: setTimes                                             
/* Purpose: takes the user input time and parses it if it can                               
/* Parameters: char* inputTime: user input time
			   int accessFlag: -a input or not
			   int modifyFlag: -m input or not
			   struct stat fileInfo: times currently set for file
			   char* currentFile: the name of the file to be updated
/* Returns: int (true/false)                  
/***********************************************/
int setTimes(char* inputTime)
{
	int timeLength = strlen(inputTime); //length of time input
	struct tm timeStruct; //time struct to hold parsed time info
	time_t currentTimeTimeT = time(NULL); //current time as a time_t object
	struct tm currentTimeTm = *localtime(&currentTimeTimeT); //current time broken up into tm struct
	
	if(timeLength == 8) //should be format MMDDhhmm
	{
		if(strptime(inputTime, "%m%d%H%M", &timeStruct))
		{
			timeStruct.tm_year = currentTimeTm.tm_year; //no year input default to current year
			timeStruct.tm_sec = 0; //no seconds set to 0
		}
		else //didn't match format
		{
			printf("Invalid Time\n");
			return 0;
		}
	}
	else if(timeLength == 10) //should be format YYMMDDhhmm
	{
		if(strptime(inputTime, "%y%m%d%H%M", &timeStruct))
		{
			timeStruct.tm_sec = 0; // no seconds set to 0
		}
		else //didn't match format
		{
			printf("Invalid Time\n");
			return 0;
		}
	}
	else if(timeLength == 12) //should be CCYYMMDDhhmm
	{
		if(strptime(inputTime, "%C%y%m%d%H%M", &timeStruct))
		{
			timeStruct.tm_sec = 0; //no seconds set to 0
		}
		else //didn't match format
		{
			printf("Invalid Time\n");
			return 0;
		}
	}
	else if(timeLength == 11) //should be MMDDhhmm.ss
	{
		if(strptime(inputTime, "%m%d%H%M.%S", &timeStruct))
		{
			timeStruct.tm_year = currentTimeTm.tm_year; //no year default to current
		}
		else //didn't match format
		{ 
			printf("Invalid Time\n");
			return 0;
		}
	}
	else if(timeLength == 13) //should be YYMMDDhhmm.ss
	{
		if(!strptime(inputTime, "%y%m%d%H%M.%S", &timeStruct)) //didn't match
		{
			printf("Invalid Time\n");
			return 0;
		}
	}
	else if(timeLength == 15) //should be CCYYMMDDhhmm.ss
	{
		if(!strptime(inputTime, "%C%y%m%d%H%M.%S", &timeStruct)) //didn't match
		{
			printf("Invalid Time\n");
			return 0;
		}
	}
	else //invalid length
	{
		return 0;
	}

	//make sure we can amke a time out of it
	if(mktime(&timeStruct) == -1)
	{
		printf("error\n");
		return 0;
	}
	else //set the converted time
	{
		convertedTime = mktime(&timeStruct);
	}

	return 1; //successful time conersion
}

/***********************************************/
/* Function: writeTimes                                             
/* Purpose: takes the input time and writes into files                              
/* Parameters: time_t inputTime: user input time as a time_t
			   int accessFlag: -a input or not
			   int modifyFlag: -m input or not
			   struct stat fileInfo: times currently set for file
			   char* currentFile: the name of the file to be updated
/* Returns: int (true/false)                  
/***********************************************/
int writeTimes(time_t inputTime, struct stat fileInfo, int accessFlag, int modifyFlag, char* currentFile)
{
	struct utimbuf newTimes; //new times to insert
	newTimes.actime = fileInfo.st_atime; //set access time to files access time
	newTimes.modtime = fileInfo.st_mtime; //set modify time to files modify time

	if(accessFlag || modifyFlag) //if it has either of the flags
	{
		if(accessFlag) //we are updating access 
		{
			newTimes.actime = time(NULL); //set to current time
		}
		if(modifyFlag)
		{
			newTimes.modtime = inputTime; //set to input time (could be current)
		}
	}
	else //neither is set so we update both
	{
		newTimes.actime = time(NULL); //access time to current time
		newTimes.modtime = inputTime; //modify time to input time (could be current)
	}
	utime(currentFile, &newTimes); //write in new times to file info
	return 1;
}

/***********************************************/
/* Function: touchOutput                                             
/* Purpose: output info about touch format                               
/* Parameters: none                          
/* Returns: int (true/false)                  
/***********************************************/
int touchOutput()
{
	//print info about format to use
	printf("Use the following format (options can come before or after):\n");
	printf("./prog3 [-a] [-m] [-c] [-t [[CC]YY]MMDDhhmm[.ss]] file \n");
}