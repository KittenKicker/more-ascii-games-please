#include <stdio.h>
#include <fcntl.h>
#include <utmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

int numStructsInFile = 0; //hold the number of structs in wtmp
char currentLine[32]; //hold the ut_line of the current struct being checked for logout
int hadSystemReboot = 0; //has the system reboot since the user logged in?
int isSystemReboot = 0; //is the current struct a system reboot?

/***********************************************
 Function: isInt                                          
 Purpose: take in a string and determine if it is an integer                              
 Parameters: char* string to be tested                          
 Returns: int  (true/false)                 
***********************************************/
int isInt(char *string)
{
	int number;
	return sscanf(string, "%d", &number);
}

/***********************************************
 Function: showTime                                             
 Purpose: format time from struct neatly and print                           
 Parameters: timeval (time to beautify)
 			 shortTime (flag to print just hh:mm or full date time)                          
 Returns: int                   
***********************************************/
void showTime(long timeval, int shortTime)
{
	char *timePointer; //time stored into this char*
	timePointer = ctime(&timeval); //ctime on the time to format nicely

	//print just the time hh:mm
	if(shortTime)
	{
		printf("%-5.5s", timePointer+11); //skips to hh:mm
	}
	//print full date time
	else
	{
		printf("%16.16s", timePointer);
	}
}

/***********************************************
 Function: isCurrentStructLogout                                             
 Purpose: take in a struct and see if it matches the global 
 	      currentLine and then it is the logout for that struct                             
 Parameters: struct utmp* utbufp (the current wtmp struct being checked)                        
 Returns: int (true/false)                  
***********************************************/
int isCurrentStructLogout(struct utmp *utbufp)
{
	//if you see a system reboot it can't still be logged in
	if(utbufp->ut_type == 2)
	{
		hadSystemReboot = 1;
	}

	//if it is a dead process and the line matches then you found logout
	if(utbufp->ut_type == 8 && strcmp(utbufp->ut_line, currentLine) == 0)
	{
		//skip printing end time if the struct is a system reboot struct
		if(isSystemReboot == 0)
		{
			printf(" - ");
			showTime(utbufp->ut_time, 1);
		}

		return 1; //true
	}
	else
	{
		return 0; //false
	}
}

/***********************************************
 Function: printInfo                                        
 Purpose: take in struct and see if it should be printed, if so print details                             
 Parameters: struct utmp* utbufp (current struct)
             name (name passed in by user if any)                          
 Returns: int                   
***********************************************/
int printInfo(struct utmp *utbufp, char *name)
{
	//if it's a user process or a reboot & the name matches if there is an input, we want to show details
	if((utbufp->ut_type == 7 || utbufp->ut_type == 2) && (strcmp(name, "") == 0 || strcmp(utbufp->ut_name,name) == 0))
	{
		//save current line detail
		strcpy(currentLine, utbufp->ut_line);
	
		//print the name of the user
		printf("%-8.8s", utbufp->ut_name);
		printf(" ");

		//if it's a user login show the line
		if(utbufp->ut_type == 7)
		{
			printf("%-12.12s", utbufp->ut_line);
		}
		//display system boot
		else
		{
			isSystemReboot = 1; //set system reboot flag since this is a reboot
			printf("%-12.12s", "system boot");
		}

		printf(" ");
		
		//make sure not null host and print it
		if(utbufp->ut_host[0] != '\0')
		{
			printf("%-16.16s", utbufp->ut_host);
			printf(" ");
		}

		//show full time for start time of process
		showTime(utbufp->ut_time, 0);
	}
	else //not a process type we want to display
	{
		return 0; //false
	}

	return 1; //true
}

/***********************************************
 Function: Main                                             
 Purpose: take in options and print off like linux 'last' does                               
 Parameters: int argc (num arguments)
 			 char** argv (arguments)
 			 	-n (required before number)
 			 	name (only print process from this user)
 			 	number (number of processes to print)
 Returns: int                   
***********************************************/

int main(int argc, char **argv)
{
	char *name = ""; //the user input name
	char numberToken[] = "-n"; //the token that comes before the number in arguments
	int lastAmount = -1; //amount user wants displayed < 0 assume no input
	struct utmp utbuf; //current struct
	int wtmpfd; //wtmp file
	int reclen = sizeof(utbuf); //length of the structs
	int structsEncountered; //number of structs parsed through
	int logoutFinder; //counter for finding logout for logins
	int logoutFound = 0; //has the logout record been found?

	//if you have all 3 options and the first is -n
	if(argc == 4 && strcmp(argv[1],numberToken) == 0)
	{
		//the second has to be a number
		if(isInt(argv[2]) == 0)
		{
			perror("Invalid argument after -n");
			exit(1);
		}

		//set last amount and name
		lastAmount = atoi(argv[2]);
		name = argv[3];
	}
	//all 3 options and -n is 2nd
	else if (argc == 4 && strcmp(argv[2], numberToken) == 0)
	{
		//third has to be a number
		if(isInt(argv[3]) == 0)
		{
			perror("Invalid argument after -n");
			exit(1);
		}

		//set last amount and name
		name = argv[1];
		lastAmount = atoi(argv[3]);
	}
	//any other combination of 3 is bad
	else if(argc == 4)
	{
		perror("Invalid arguments");
		exit(1);
	}
	//2 arguments and -n is first
	else if(argc == 3 && strcmp(argv[1], numberToken) == 0)
	{
		//second has to be an int
		if(isInt(argv[2]) == 0)
		{
			perror("Invalid argument after -n");
			exit(1);
		}
		//only set last amount
		lastAmount = atoi(argv[2]);
	}
	//any other combination of 2 options is bad
	else if(argc == 3)
	{
		perror("Invalid arguments");
		exit(1);
	}
	//only 1 option
	else if(argc == 2)
	{
		//has to be name
		name = argv[1];
	}
	//too many arguments is bad
	else if(argc > 4)
	{
		perror("Invalid arguments");
		exit(1);
	}

	wtmpfd = open(WTMP_FILE, O_RDONLY);//open the file
	if (wtmpfd == -1)//make sure open worked
	{
		perror("Error opening WTMP");
		exit(1);
	}

	//count the number of structs in file
	while(read(wtmpfd, &utbuf, reclen) == reclen)
	{
		numStructsInFile++;
	}

	//close and reopen to be safe
	close(wtmpfd);
	wtmpfd = open(WTMP_FILE, O_RDONLY);
	
	//walk through all of the structs in the file
	for(structsEncountered = 1; structsEncountered <= numStructsInFile; structsEncountered++)
	{
		//if you printed off all you need stop
		if(lastAmount == 0)
		{
			break;
		}

		//this allows for looking at last struct in file first
		lseek(wtmpfd, sizeof(struct utmp) * (numStructsInFile - structsEncountered), SEEK_SET);

		//if you read the record succesffully
		if(read(wtmpfd, &utbuf, reclen) == reclen)
		{
			//print the info if it's a good struct
			if(printInfo(&utbuf, name))
			{
				//find if here is a logout for the printed struct
				//only looking at records newer than the current sstruct
				for(logoutFinder = structsEncountered-1; logoutFinder > 0; logoutFinder--)
				{
					//work from record closest to the time of the current struct to newest
					lseek(wtmpfd, sizeof(struct utmp) * (numStructsInFile - logoutFinder), SEEK_SET);
					read(wtmpfd, &utbuf, reclen); //read the struct you jumped to
					
					if(isCurrentStructLogout(&utbuf)) //see if it's the logout or not
					{
						logoutFound = 1; //set flag
						break; //stop looking
					}
					else
					{
						if(hadSystemReboot)
						{
							break;
						}
					}
				}

				if(logoutFound == 0) //if you didn't find a logout
				{
					if(isSystemReboot == 0) //if it's not a reboot
					{
						if(hadSystemReboot) //if there was a system reboot it crashed
						{
							printf(" - crash");
						}
						else //if not it's still logged in
						{
							printf(" still logged in");
						}
					}
				}

				//reset the flags
				isSystemReboot = 0;
				logoutFound = 0;
				hadSystemReboot = 0;

				//decrement last amount to see if you printed all the user wanted
				lastAmount--;

				//make a new line for next struct
				printf("\n");
			}
		}
	}

	//close the file
	close(wtmpfd);
	return 0;
}