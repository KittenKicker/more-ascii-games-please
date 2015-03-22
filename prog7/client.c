/***********************************************/
/* Devin Holland                                             
/* Login ID: holl4332                               
/* CS-202, Winter 2015                          
/* Programming Assignment 7                  
/* client for playing games of sine herd pig                    
/***********************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

/***********************************************/
/* Function: connect_to_server                                          
/* Purpose: make connection to the socket                        
/* Parameters: name of host and port number                    
/* Returns: int socket
/*********************************************/
int connect_to_server(char *host, int portnum) 
{
    int sock; //the socket

    struct sockaddr_in servadd; //server address
    struct hostent *hp; //host

    //open socket
    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock == -1 ) 
    {
        return -1;
    }

    //zero out server address
    bzero( &servadd, sizeof(servadd) );

    //get hp from host name
    hp = gethostbyname( host );
    if (hp == NULL)
    { 
        return -1;
    }

    //set up server address info
    bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr,
    hp->h_length);
    servadd.sin_port = htons(portnum);
    servadd.sin_family = AF_INET ;

    //connect to the socket
    if ( connect(sock,(struct sockaddr *)&servadd,
        sizeof(servadd)) !=0) 
    {
        return -1;
    }

    return sock;
}

/***********************************************/
/* Function: getMove                                         
/* Purpose: get players move of move                         
/* Parameters:none                      
/* Returns: int move number          
/*********************************************/
int getMove() 
{
  printf("sit or stand?\n"); //ask user move
  char move[16]; //read move
  int index = 0; //index for making word lowercase

  fgets(move, sizeof(move), stdin); // get move

  //make move input lowercase
  for(index = 0; move[index]; index++) 
  {
    move[index] =  tolower(move[index]);
  }

  //check if they typed hold or sit
  if((strncmp("hold", move, strlen("hold")) == 0) || (strncmp("sit", move, strlen("sit")) == 0)) 
  {
    printf("(will wait for other player to finish round if necessary)\n");
    return 0;
  } 
  //check if they rolled or stand
  else if((strncmp("roll", move, strlen("roll")) == 0) || (strncmp("stand", move, strlen("stand")) == 0)) 
  {
    return 1;
  } 
  else //they input something dumb
  {
    printf("Please pick sit or stand\n");
    memset(move, 0, sizeof(move));
    return -1;
  }
}

/***********************************************/
/* Function: playGame                                          
/* Purpose: handles input and output for playing game                        
/* Parameters:int file descriptor for client                     
/* Returns: none           
/*********************************************/
playGame(int fd)
{
    char buf[1024]; //input from server
    char userMove[256]; //user's move choice
    int nbytes; //number of bytes read
    int gameOver = 0; //is the game over
    int move; //move of player

    //read from server
    nbytes = recv(fd, buf, sizeof(buf),0);
    buf[nbytes] = '\0';

    //first read is opponents username
    printf("Your opponent is: %s\n", buf);
    //send confirmation
    write(fd, "good", 4);
    //clear buffer
    memset(buf, 0, sizeof(buf));

    while(!gameOver) //game is still going
    {
        //read next input
        nbytes = read(fd, buf, sizeof(buf));
        buf[nbytes] = '\0';

        //roll announcement
        if(strncmp("Roll was", buf, strlen("Roll was")) == 0)
        {
            //print roll and confirm
            printf("%s\n", buf);
            write(fd, "good", 4);
        }
        // opponent move announcement
        else if(strncmp("Opponent's", buf, strlen("Opponent's"))==0)
        {
            //print move and confirm
            printf("%s\n", buf);
            write(fd, "good", 4);
        }
        //score update
        else if(strncmp("Player 1 Score", buf, strlen("Player 1 Score")) == 0)
        {
            //print score and confirm
            printf("%s\n", buf);
            write(fd, "good", 4);
        }
        //get move request
        else if(atoi(buf) == 777)
        {
            do //prompt user till valid response
            {
                move = getMove();
            } while(move == -1);

            //write move to server
            sprintf(userMove, "%d", move);
            write(fd, userMove, sizeof(move));

            //clear move buffer
            memset(userMove, 0 , sizeof(move));
        }
        //game is over
        else if(strncmp("Game over", buf, strlen("Game over")) == 0)
        {
            //print winner and confirm
            printf("%s\n",  buf);
            write(fd, "good", 4);

            //set game over flag
            gameOver = 1;
        }

        //clear buffer
        memset(buf, 0, sizeof(buf));
    }
    return;
}

/***********************************************/
/* Function: sendUsername                                          
/* Purpose: write username to serever                        
/* Parameters:int file descriptor and username                    
/* Returns: none           
/*********************************************/
sendUsername(int fd, char username[256])
{
    //write username to server and start playing
    write(fd, username, strlen(username));
    playGame(fd);
}

/***********************************************/
/* Function: process_reqeust                                          
/* Purpose: handle starting up and playing game                         
/* Parameters:int file descriptor for each player                     
/* Returns: none           
/*********************************************/
int main(int argv, char *argc[]) 
{
    int fd; //file descriptor for client
    char username[256]; //inpt username
    
    //connect to server
    fd = connect_to_server("127.0.0.1", 5123);

    if(fd == -1) //couldn't connect
    {
        exit(1);
    }
    //prompt for username
    printf("Enter your username\n");
    fgets(username, 256, stdin);
  
    //send username and start playing
    sendUsername(fd, username);
    close (fd);

    return 0;
}