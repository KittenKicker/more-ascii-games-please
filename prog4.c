/***********************************************/
/* Devin Holland                                             
/* Login ID: holl4332                               
/* CS-202, Winter 2015                          
/* Programming Assignment 4                        
/* Loops forever and prints info when sigint is caught                    
/***********************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

long long highestPrime; //highest found prime number
int numPrimesFound = 0; //number of primes found

/***********************************************/
/* Function: signalHandler                                            
/* Purpose: print off current info when sigint is sent                               
/* Parameters: none                          
/* Returns: nothing                   
/***********************************************/
void signalHandler()
{
	printf("\nNumber of Primes Found: %d\nHighest Prime Found: %lld\n", numPrimesFound, highestPrime);
	return;
}

/***********************************************/
/* Function: Main                                             
/* Purpose: loops through numbers forever checking for primality
            printing handled by signal catching                               
/* Parameters: None                          
/* Returns: int                   
/***********************************************/
int main()
{
	long long currentNumber = 2; //number being checked for primality
	long long primalityChecker; //counter for brute force algorithm
	int isPrime; //flag if number is prime

	//call signalHandler function when sigint is sent
	signal(SIGINT, signalHandler); 

	while(1) //loop forever
	{
		isPrime = 1; //set primality flag to true

		//loop from 2 (1 is always divisble) to the number 1 less than current 
		for(primalityChecker = 2; primalityChecker<currentNumber; primalityChecker++)
		{
			//if its divisible it can't be a prime
			if(currentNumber % primalityChecker == 0 )
			{
				isPrime = 0; //set primality flag to false;
				break; //skip out of the for loop;
			}
		}

		//if you made it through the loop without a divisor it's prime
		if(isPrime == 1)
		{
			//increase number of primes found
			numPrimesFound++;

			//update highest prime to current number 
			highestPrime = currentNumber;
		}
		//move to check next number
		currentNumber++;
	}

	return 0;
}