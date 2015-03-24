/***********************************************/
/* Devin Holland                                             
/* Login ID: holl4332                               
/* CS-202, Winter 2015                          
/* Programming Assignment 7                   
/* Server for running games of swine herd pig                    
/***********************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#define HOSTLEN 256
#define BACKLOG 1

/***********************************************/
/* Function: make_server_socket_q (defined later)               
/***********************************************/
int make_server_socket_q(int , int );

/***********************************************/
/* Function: make_server_socket                                           
/* Purpose: take port and backlog & cal
/*     make server socket q                              
/* Parameters: int portnum                        
/* Returns: int socket id                  
/*********************************************/
int make_server_socket(int portnum) 
{
 return make_server_socket_q(portnum, BACKLOG);
}

/***********************************************/
/* Function: make_server_socket_q                                           
/* Purpose: take port and backlog & create socket                           
/* Parameters: int portnum, int backlog                      
/* Returns: int socket id                  
/*********************************************/
int make_server_socket_q(int portnum, int backlog) 
{
    struct sockaddr_in saddr; //socket address
    struct hostent *hp; //host
    char hostname[HOSTLEN];  //name of host
    int sock_id; //id of socket created

    //creae socket and check that it worked
    sock_id = socket(PF_INET, SOCK_STREAM, 0);
    if ( sock_id == -1 ) 
    {
        return -1;
    }

    //zero out socket address
    bzero((void *)&saddr, sizeof(saddr));

    //set socket address port and family
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(portnum);
    saddr.sin_family = AF_INET ;

    //bind info to socket id
    if ( bind(sock_id, (struct sockaddr *)&saddr,
        sizeof(saddr)) != 0 ) 
    {
        return -1;
    }

    //set up listener on socket
    if ( listen(sock_id, backlog) != 0 ) 
    {
        return -1;
    }

    return sock_id;
}

/***********************************************/
/* Function: disconnectWinner                                          
/* Purpose: let other player know they won from Disconnect                              
/* Parameters: int file descriptor for winning player                       
/* Returns: none
/*********************************************/
disconnectWinner(int fd)
{
    char buffer[256];
    sprintf(buffer, "Other player disconnected. You win!");
    write(fd, buffer, sizeof(buffer));
    recv(fd, buffer, 4, 0);
    exit(0);
}

/***********************************************/
/* Function: getRandomNumber                                           
/* Purpose: generate random number 1-6                              
/* Parameters: none                       
/* Returns: int randomized number                 
/*********************************************/
int getRandomNumber() 
{
  int randomNumber; //the number
  //get 1-6
  randomNumber = (rand() % 6) + 1;

  return randomNumber;
}

/***********************************************/
/* Function: isGameOver                                          
/* Purpose: tell if anyone has won and if so who                              
/* Parameters: int first players score, second players score                       
/* Returns: int who won   (-1 if not over)                
/*********************************************/
int isGameOver(int p1Points, int p2Points) 
{
  if(p1Points >= 100 && p1Points > p2Points)
  {
    return 0; //player 1 won
  }
  else if(p2Points >= 100 && p2Points > p1Points)
  {
    return 1; //player 2 won
  }
  else if(p2Points >= 100 && p2Points == p1Points)
  {
    return 2; //tie game
  }
  else
  {
    return -1; //games not over
  }
}

/***********************************************/
/* Function: sendTotalScore                                          
/* Purpose: send scores to client                          
/* Parameters: int player 1 poitns, player 2 points, 
/*             file descriptor                      
/* Returns: int socket id                  
/*********************************************/
int sendTotalScore(int p1Points, int p2Points, int fd) 
{
  char buffer[256]; //set up string to send
  sprintf(buffer, "Player 1 Score: %d Player 2 Score: %d\n", p1Points, p2Points);

  //send to client
  write(fd, buffer, sizeof(buffer));

  //confirm client got & printed scores
  if(recv(fd, buffer, 4, 0) <= 0)
  {
    return -1;
  }

  return 0;
}

/***********************************************/
/* Function: sendMove                                           
/* Purpose: send info if their opponent is still standing                           
/* Parameters:int file descriptor                      
/* Returns: int                
/*********************************************/
int sendMove(int fd)
{
    char buffer[256]; //set up striing to send
    //the opponent is still standing and hasnt sat yet
    sprintf(buffer, "Opponent's choice: Still Standing");

    //send to client
    write(fd, buffer, sizeof(buffer));

    //wait for confirmation from client
    if(recv(fd, buffer, 4, 0) <= 0)
    {
        return -1;
    }
    return 0;
}

/***********************************************/
/* Function: sendRoll                                          
/* Purpose: tell clients what the roll was                           
/* Parameters:int roll,  file descriptor                      
/* Returns: int                
/*********************************************/
int sendRoll(int roll, int fd) 
{
  char buffer[256]; //hold string to send

  sprintf(buffer, "Roll was: %d\n", roll);

  //send roll to client
  write(fd, buffer, sizeof(buffer));

  //wait for confirmation
  if(recv(fd, buffer, 4,0) <= 0)
  {
    return -1;
  }

  return 0;
}

/***********************************************/
/* Function: getMove                                           
/* Purpose: send request to client for move                          
/* Parameters:int file descriptor                  
/* Returns: int     chosen move           
/*********************************************/
int getMove(int fd) 
{
  //ftruncate(fd,0);
  char buffer[256]; //hold signal to send
  int getMoveSignal = 777; 
  // 777 protocool for request

  int nbytes; //number of bytes receied

  sprintf(buffer, "%d", getMoveSignal);

  //send request to client
  write(fd, buffer, sizeof(buffer));

  //clear buffer and wait for response
  memset(buffer, 0, sizeof(buffer));
  nbytes = read(fd, buffer, sizeof(buffer));

  if(nbytes <= 0)
  {
    return -7; //number for sure not in use
  }

  //null terminate buffer
  buffer[nbytes] = '\0';

  //returns the number chosen for move
  return atoi(buffer);
}

/***********************************************/
/* Function: playGame                                          
/* Purpose: run the game                          
/* Parameters: file descriptors for player 1 and 2                      
/* Returns: int                
/*********************************************/
int playGame(int fdp1, int fdp2) 
{
    //set up total and round scoreas
  int p1TotalScore = 0;
  int p2TotalScore = 0;
  int p1RoundScore = 0;
  int p2RoundScore = 0;

  //set moves to stand
  int p1Move = 1;
  int p2Move = 1;

  //the die roll
  int roll = 0;

  //is game over?
  int gameOverResult = -1;

  //while the game is not over
  while(gameOverResult == -1) 
  {
    //get a new roll and send to users
    roll = getRandomNumber();

    if(sendRoll(roll, fdp1) == -1)
    {
        disconnectWinner(fdp2);
    }

    if(sendRoll(roll, fdp2) == -1)
    {
        disconnectWinner(fdp1);
    }

    //if roll is 1 set player round score to 0 if they were
    //standing, otherwise add to their total score
    if(roll == 1) 
    {
      if(p1Move == 1) 
      {
        p1RoundScore = 0;
      }
      if(p2Move == 1) 
      {
        p2RoundScore = 0;
      }
      p1TotalScore = p1TotalScore + p1RoundScore;
      p2TotalScore = p2TotalScore + p2RoundScore;

      //reset round score
      p1RoundScore = 0;
      p2RoundScore = 0;

      //send total score update
      if(sendTotalScore(p1TotalScore, p2TotalScore, fdp1) == -1)
      {
        disconnectWinner(fdp2);
      }

      if(sendTotalScore(p1TotalScore, p2TotalScore, fdp2) == -1)
      {
        disconnectWinner(fdp1);
      }

      //reset move to stand
      p1Move = 1;
      p2Move = 1;

      //see if game is over
      gameOverResult = isGameOver(p1TotalScore, p2TotalScore);
    } 
    else  //roll is not 1
    {
      //if the player is standing add points to their round score
      if(p1Move == 1) 
      {
        p1RoundScore = p1RoundScore + roll;
      }
      if(p2Move == 1) 
      {
        p2RoundScore = p2RoundScore + roll;
      }
    }

    //if game isn't over and the roll wasn't 1
    if(gameOverResult == -1 && roll != 1) 
    {
        //if player isn't sitting, ask for next move
        if(p1Move != 0)
        {
            p1Move = getMove(fdp1);
            if(p1Move == -7)
            {
                disconnectWinner(fdp2);
            }
        }
        if(p2Move != 0)
        {
            p2Move = getMove(fdp2);
            if(p2Move == -7)
            {
                disconnectWinner(fdp1);
            }
        }      

        //let player 2 know player 1 is still standing
        if(p1Move != 0)
        {
            if( sendMove(fdp2) == -1)
            {
                disconnectWinner(fdp1);
            }
        }

        //let player 1 know player 2 is still standing
        if(p2Move != 0)
        {
            if(sendMove(fdp1) == -1)
            {
                disconnectWinner(fdp2);
            }
        }

        //if both players sat
        if(p1Move == 0 && p2Move == 0) 
        {
            //add to total score
            p1TotalScore = p1TotalScore + p1RoundScore;
            p2TotalScore = p2TotalScore + p2RoundScore;

            //reset round score and send score updates
            p1RoundScore = 0;
            p2RoundScore = 0;
            if(sendTotalScore(p1TotalScore, p2TotalScore, fdp1) == -1)
            {
                disconnectWinner(fdp2);
            }

            if(sendTotalScore(p1TotalScore, p2TotalScore, fdp2) == -1)
            {
                disconnectWinner(fdp1);
            }

            //reset players to standing
            p1Move = 1;
            p2Move = 1;

            //check if game is over
            gameOverResult = isGameOver(p1TotalScore, p2TotalScore);
      }
    }
  }
  return gameOverResult; //who won/tie
}

/***********************************************/
/* Function: process_reqeust                                          
/* Purpose: handle starting up and playing game                         
/* Parameters:int file descriptor for each player                     
/* Returns: none           
/*********************************************/
process_request(int fd, int fd2) 
{
    //response from client
    char response[5];

    //username lengths
    int username1Length;
    int username2Length;

    //usernames
    char username[256];
    char username2[256];

    //buffer & clearing
    char buf[256];
    memset(buf, 0, 256);

    //result of game
    int result;

    //read in usernames
    username1Length = recv(fd, username, sizeof(username), 0);
    
    username2Length = recv(fd2, username2, sizeof(username2), 0);
    
    //null terminate usernames
    username[username1Length] = '\0';
    username2[username2Length] = '\0';

    //print opponent to clients
    printf("Connected: %s\n", username);
    write(fd2, username, strlen(username));
    printf("Connected: %s\n", username2);
    write(fd, username2, strlen(username2));

    //wait for confirmation from clients

    if(recv(fd2, response, 4, 0) <= 0)
    {
        disconnectWinner(fd);
    }
    
    if(recv(fd, response, 4, 0) <= 0)
    {
        disconnectWinner(fd2);
    }

    //start the game and wait for who won
    result = playGame(fd, fd2);

    //player 1 won
    if(result == 0)
    {
        //tell clients who won
        sprintf(buf, "Game over, %s won the game.", username);
        write(fd, buf, strlen(buf));
        write(fd2, buf, strlen(buf));

        //wait for confirmation
        recv(fd, response, 4, 0);
        recv(fd2, response, 4, 0);

        //close file descriptors and exit
        close(fd);
        close(fd2);
        exit(0);
    }
    //player 2 won
    else if( result == 1)
    {
        //tell clients who won
        sprintf(buf, "Game over, %s won the game.", username2);
        write(fd, buf, strlen(buf));
        write(fd2, buf, strlen(buf));

        //wait for confirmation
        recv(fd, response, 4, 0);
        recv(fd2, response, 4, 0);

        //close file descriptors and exit
        close(fd);
        close(fd2);
        exit(0);
    }
    //tie
    else if(result == 2)
    {
        //tell clients it's a tie
        sprintf(buf, "Game over, It's a tie game.");
        write(fd, buf, strlen(buf));
        write(fd2, buf, strlen(buf));

        //wait for confirmation
        recv(fd, response, 4, 0);
        recv(fd2, response, 4, 0);

        //close file descriptors and exit
        close(fd);
        close(fd2);
        exit(0);
    }
    return;
}

/***********************************************/
/* Function: main                                           
/* Purpose: create child processes to play games in                         
/* Parameters:none                     
/* Returns: int                
/*********************************************/
int main() 
{
    int sock; //socket id

    sock = make_server_socket(5123); //create socket
    if (sock == -1) 
    { //check that it worked
        perror("Error opening socket");
        exit(1);
    }
    while(1) //wait for multiple games
    {
        srand(time(NULL)); //set up random generator
        
        int fd; //file descriptor 1
        int fd2; // file descriptor 2
        int pid; //process id

        //accept sockets
        fd = accept(sock, NULL, NULL);
        if ( fd == -1) 
        {
            return;
        }
        fd2 = accept(sock, NULL, NULL);
        if (fd2 == -1) 
        {
            return;
        }

        pid = fork(); //fork for a new game
        //play the game
        if(pid == 0)
        {
            process_request(fd, fd2);
        }
        else if(pid == -1)
        {
            perror("couldn't fork");
        }
    }
    
    return 0;
} 