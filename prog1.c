/***********************************************
 Devin Holland                                             
 Login ID: holl4332                               
 CS-202, Winter 2015                          
 Programming Assignment 1                         
 Prints off double-base palindromes up to entered value                    
***********************************************/

#include <stdio.h>

/***********************************************
 Function: isPalindrome                                             
 Purpose: take in a long long and determines if it is a palindrome
 		The reason for this is because the program represents binary as
 		a long long so integer reversal can be performed.
 Parameters: a long long "currentNumber" that represents either the decimal or binary number                          
 Returns: int
***********************************************/
int isPalindrome(long long currentNumber)
{
	//since the number is divided down to 0, we need something to hold the
	//original number to check the reversed number against
	long long reversalHolder = currentNumber;
	long long reversedNum = 0; //stores the reversed number
	int onesPlace; //the number in the ones place after each division

	//Loops through the digits of the original number and adds them to 
	
	//reversed number in the opposite order
	while(reversalHolder >= 1)
	{
		onesPlace = reversalHolder % 10; //get ones digit
		reversedNum = reversedNum * 10 + onesPlace; //add it to reversed number
		reversalHolder = reversalHolder / 10; //drop the digit we already added
	}

	return (currentNumber == reversedNum); //see if it's a palindrome
}

/***********************************************
 Function: Main                                             
 Purpose: take in user input and print double-base palindrome outputs                               
 Parameters: None                          
 Returns: int                   
***********************************************/
int main(void)
{	
	int userInput; //value entered by user
	int currentNumber; //number currently being checked

	//Current number stored in long long represented in binary
    long long currentNumberInBinary;

	int total = 0; //the total of the double-base palindromes

	int currentBit; //the current bit (0/1) in the current number being checked
	int bitIndex; //the place of the bit to be grabbed for currentBit
	int intLength; //holds the length of the integer in bits

	intLength = sizeof(int) * 8; //multiply by 8 to convert bytes -> bits

	printf("Enter value of N: ");
	scanf("%d", &userInput); //grab user number
	printf("\n");

	//loop through numbers from 0 to the user input number
	for(currentNumber = 0; currentNumber <= userInput; currentNumber++)
	{
		//check if the number in Decimal is a palindrome
		if(isPalindrome(currentNumber))
		{
			currentNumberInBinary = 0; //reset current binary number

			//loop through bits of number.
			for(bitIndex = intLength-1; bitIndex >= 0; bitIndex--)
			{
				//right shift based on index (starting to look at right most bit)
				currentBit = currentNumber >> bitIndex;
				
				//move the previous added bits left one
				currentNumberInBinary = currentNumberInBinary * 10;

				if(currentBit & 1)
				{
					//if the current bit is 1 then add into binary num.
					currentNumberInBinary += 1;
				}
			}

			//check completed binary number to see if it's a palindrome
			if(isPalindrome(currentNumberInBinary))
			{
				//print and add to total
				printf("%d & %lld\n", currentNumber, currentNumberInBinary);
				total += currentNumber;
			}

		}
	}

	//print the sum
	printf("The total of the palindromes is: %d\n", total);

	return 0;
}