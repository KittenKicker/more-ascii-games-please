/***********************************************/
/* Devin Holland                                             
/* Login ID: holl4332                               
/* CS-202, Winter 2015                          
/* Programming Assignment 5                      
/* Scrolls text from file accross screen                    
/***********************************************/

#include <curses.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

/***********************************************/
/* Function: makeScrollingText                                            
/* Purpose: scrolls the text accross the screen after adding necessary blanks                              
/* Parameters: inputFileName (the file name); LoopingFlag (are we looping forever);
			   delayTimer (time between characters)                      
/* Returns: int                   
/***********************************************/
int makeScrollingText(char* inputFileName, int loopingFlag, int delayTimer)
{
	FILE *file; //the file pointer for the file
	char *inputFileBuff; //buffer for the file
	long lengthOfFile; //number of characters in teh file
	file = fopen(inputFileName, "r"); //open the file
	long scrollingCounter; //counter for scrolling the text
	int blanksToUse = 0;//blanks needed to fill up screen after text goes off screen
	int charactersOffScreen =0; //num characters off the screen on the left
	int specialCharacterChecker = 0; //counter for checking character

	int maxX; //max number of x columns 
	int maxY; //max number of y rows

	initscr(); //initialize the screen
	getmaxyx(stdscr, maxY, maxX); //get the max values

	curs_set(0);//hide cursor

	if(file != NULL) //see if you got a file back
	{
		if(fseek(file, 0, SEEK_END) == 0) //make sure you can go to the end of the file
		{
			lengthOfFile = ftell(file);//how many bytes are in this
			inputFileBuff = (char*)malloc(sizeof(char) *( lengthOfFile +maxX+ 3)); 
			//set the buffer to the right size
			//the +3 is needed for the null char and extra for going off of screen on left during looping
			
			fseek(file, 0, SEEK_SET); //go back to beginning of file

			if(fread(inputFileBuff, 1, lengthOfFile, file) == 0)//try to read file
			{
				perror("Nothing was able to be read");
			}
			else
			{
				//loop through and replace enter and tab characters with ' ';
				for(specialCharacterChecker = 0; 
					specialCharacterChecker<lengthOfFile; 
					specialCharacterChecker++)
				{
					if(inputFileBuff[specialCharacterChecker] == '\n' 
						|| inputFileBuff[specialCharacterChecker] == '\t')
					{
						inputFileBuff[specialCharacterChecker] = ' ';
					}
				}

				//add blanks to fill up screen after your text is done coming in from the right
				for(blanksToUse=0; blanksToUse<=maxX; blanksToUse++)
				{
					inputFileBuff[lengthOfFile+blanksToUse] = ' ';
				}
				//null terminate string
				inputFileBuff[lengthOfFile+2] = '\0';
			}
		}
	}
	else
	{
		perror("Unable to open file");
	}

	do //this happens at least once or multiple times if loopingFlag is set
	{
		//reset characters off the screen
		charactersOffScreen = 0;

		//start scrolling from beginning of file through the rest of the file and blanks
		for(scrollingCounter = 0; scrollingCounter < lengthOfFile+maxX; scrollingCounter++)
		{
			//you have characters going off of the screen on the left
			if(scrollingCounter>=maxX-1)
			{
				charactersOffScreen++;
			}

			//if your whole window is full
			if(scrollingCounter+1 >= maxX)
			{

				//if charactrs are off screen always start at 0 pos in x
				if(charactersOffScreen > 0)
				{
					mvaddnstr(maxY/2, 0, &inputFileBuff[charactersOffScreen], maxX);
				}
				else
				{
					mvaddnstr(maxY/2, maxX-scrollingCounter-1, &inputFileBuff[charactersOffScreen], maxX);
				}
			}
			else//still room in window
			{
				mvaddnstr(maxY/2, maxX-scrollingCounter-1, &inputFileBuff[0+charactersOffScreen], scrollingCounter+1);
			}
			refresh();//refresh the screen
			usleep(delayTimer * 1000);//wait for input or default miliseconds
		}
	} while(loopingFlag);//keep looking forever
	endwin(); //close the window
	fclose(file); //close the file
	return 0;
}

int main(int argc, char** argv)
{
	int option; //option currently being checked
	int loopingFlag = 0; //are we looping forever
	int delayTimer = 1000; //delay amount in miliseconds
	char *inputFileName; //

	if(argc == 1) //if the only argument is the program name
	{
		perror("Please specify file name and valid options");
		return 1;
	}


	//parse out the options
	while((option = getopt(argc, argv, "t:l")) != -1)
	{
		switch(option)
		{
			case 't':
				delayTimer = atoi(optarg);
				break;
			case 'l':
				loopingFlag = 1;
				break;
			default:
				perror("Invalid options");
				exit(1);
		}
	}

	//set file name and check to see that it exists before trying to scroll it
	inputFileName = argv[optind];
	struct stat statbuff;
	if(stat(inputFileName, & statbuff) == 0)
	{
		makeScrollingText(inputFileName, loopingFlag, delayTimer);
	}
	else
	{
		perorr("Invalid File");
	}

	return 0;
}
