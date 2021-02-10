#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*
	The game itself loads into the console window. The user and 
	a partner take turns picking Columns to drop their pieces into
	until a line of four is created horizontally, vertically, or
	diagonally. When someone completes a  line of four they are declared
	the winner and you can choose to play again or quit.
	If the there are no more spots to drop a piece then a draw is declared.
*/

// Global Vars
char *board;
const int BRD_ROWS = 6;
const int BRD_COLS = 7;
char plyr1Name[15];
char plyr2Name[15];
int nextTurn = 0;
int quit = 0;

// Forward Declarations
void init();
void tearDown();
char acceptInput();
int checkFullBoard(char *);
int updateWorld(char);
int isWinner(char *);
int compareSpaces( char *, int, int, int, int);
int horizontalWin(char *);
int verticalWin(char *);
int diagonalWin(char *);
void displayWorld(char *);

int main(int argc, char **argv)
{

	// initialize
	init();
	char input;
	int outcome;
	displayWorld(board);

	// game loop
	// interior if-else statement outputs a message based on the
	// outcome of the players input
	while(!quit){
		input = acceptInput();
		outcome = updateWorld(input);
		
		// outcome == -2 means the board is full
		// outcome == -1 means a player chose to quit
		if(outcome == -2)
			quit++;
		else if(outcome == -1 && quit &&!nextTurn)
			printf("%s quit the game!", plyr1Name);
		else if(outcome == -1 && quit && nextTurn)
			printf("%s quit the game!", plyr2Name);
		else if(outcome == -1 && !quit)
		{
			printf(" You try to jam your piece into a full column.");
			printf(" Yet, it inevitably falls to the floor.");
			continue;
		}
		displayWorld(board);
		// outcome == 1 or 2 means someone won the game, winner determined by val
		if (outcome == -2)
			printf("The board is full! Its a Draw!\n");
		else if(outcome == 1)
			printf("%s wins the game!\n", plyr1Name);
		else if(outcome == 2)
			printf("%s wins the game!\n", plyr2Name);		
		if(!nextTurn)
			nextTurn = 1;
		else
			nextTurn = 0;
	}
	// destory game
	tearDown();

	return 0;
}

void init()
{
	// get player names initialize board.
	printf("Setting up the game...\n");
	printf("Welcome to connect four! Press 'q' in row select to quit.\n");
	printf("If game doesn't respond after entering a row, try entering in same letter again.\n");
	printf("Please, enter the name of player one: \n");
	scanf("%s", plyr1Name);
	printf("Please, enter the name of player two: \n");
	scanf("%s", plyr2Name);

	printf("\e[1;1H\e[2J");   // clears screen
	while(getchar() != '\n'); // clears input buffer

	board = (char *)malloc(sizeof(char) * BRD_ROWS * BRD_COLS);
	memset(board, ' ', BRD_ROWS * BRD_COLS);
}

void tearDown()
{
	// free board ptr
	free(board);
	printf("Destroying the game...\n");
}

/*
	Handles acceptance of players input
	loops until player picks a row or presses q to quit
	forces upper-case letters if player inputs lowercase
*/
char acceptInput()
{
	if(!nextTurn)
		printf("%s: ", plyr1Name);
	else
		printf("%s: ", plyr2Name);
	char playerInput;
	printf("Pick a row A-G!\n");
	scanf("%c", &playerInput, sizeof(char));
    // while loop until player submits an appropriate letter
	while((playerInput < 'a' || playerInput > 'g') &&
		  (playerInput < 'A' || playerInput > 'G') &&
			playerInput != 'Q' && playerInput != 'q')
	{
		printf("Input invalid >:( try again... \n");
		while(getchar() != '\n');
		scanf("%c", &playerInput);
	}

	// if letter is lowercase, make upper case
	if(playerInput >= 'a' && playerInput <= 'g' ||
			playerInput == 'q')
	{
		playerInput = playerInput - 32;
	}

	while(getchar() != '\n');
	return playerInput;
}

/*
	Updates the board based on value returned from acceptInput()
	returns -1 if player quit,  -2 if board is full
	0 if input is accepted and there is no winner yet
	and then physically puts a character into the board array
	returns 1 or 2 if player 1 or player 2 wins
*/
int updateWorld(char input)
{


	int col = -1;
	char onBoard = '/0';
	char *boardPtr;
	boardPtr = board;


		// switch statement to handle each column
	switch(input){
		case 'A':
			col = 0;
			break;
		case 'B':
			col = 1;
			break;
		case 'C':
			col = 2;
			break;
		case 'D':
			col = 3;
			break;
		case 'E':
			col = 4;
			break;
		case 'F':
			col = 5;
			break;
		case 'G':
			col = 6;
			break;
		case 'Q':
			quit = 1;
			break;

	}// end switch statement

	if(quit)
		return -1;
	
	// loop to put a temp board pointer at the proper place in board array
	//  if a character is already in the column chosen, it goes to next free
	// row in column
	boardPtr += col;
	onBoard = *boardPtr;
	int index = BRD_ROWS;
	while (onBoard != ' ' && index > 0)
	{
		boardPtr += 7;
		onBoard = *boardPtr;
		index--;
	}
	// loop failed, error, quit
	if(index == 0)
		return -1;
	
	// check if board is full
	if( checkFullBoard( board) == 1)
		return -2;
	
	// places proper character in board pointer
	if(!nextTurn)
		*boardPtr = 'X';
	else if (nextTurn)
		*boardPtr = 'O';
	
	// check for winner
	int win = isWinner(board);
	if(win > 0){
		quit = 1;
		return win;
	}
    return 0;

}

/*
	loops through board to check if full
	stops at empty space, and if the loops index
	is equal to number of spaces (-1 for zero) then board is full
*/
int checkFullBoard(char *boardState)
{
	char *brdPtr = (char *)malloc(sizeof(char));
	brdPtr = boardState;
	int i = 0;
	int retVal = -1;
	while (*brdPtr != ' ' && i != BRD_ROWS * BRD_COLS)
	{
		brdPtr ++;
		i++;
	}		
	
	if(i == BRD_ROWS * BRD_COLS - 1)
		retVal = 1;
	else
		retVal = 0;
	
	return retVal;
}

/*
	iterates through win conditions
*/
int isWinner(char *boardState)
{
	int hWin = horizontalWin(boardState);
	int vWin = verticalWin(boardState);
	int dWin = diagonalWin(boardState);
	int val = -1;
	if(hWin  == 1 || vWin == 1 || dWin == 1)
		val = 1;
	else if(hWin  == 2 || vWin == 2 || dWin == 2)
		val = 2;

	return val;
}

/*
	adds the integers passed to the board pointer, and if after the summations 
	the pointers are all pointing to similar characters then we have a winner
	returns -1 if pointer + 1st integer is blank (shouldn't happen)
	returns 0 if one of the pointers has a different character in its space than the others
	returns 1 if all pointers hold similar values
*/
int compareSpaces(char *boardState, int first, int second, int third, int fourth){
	int retVal = 0;
	char *boardPtr = (char*)malloc(sizeof(char));
	boardPtr = boardState;
	if(*(boardPtr + first) == ' ')
		retVal = -1;
	else if(*(boardPtr + first) == *(boardPtr + second) &&
		*(boardPtr + second) == *(boardPtr + third) &&
		*(boardPtr + third) == *(boardPtr + fourth))
		retVal = 1;
	return retVal;
}

/*
	loops through the left half of the board looking for potential
	horizontal straights. a potential straight is found when any character is found, 
	and then it sends that character with the next four spots in the array to the 
	compareSpaces() function. 
*/
int horizontalWin(char *boardState)
{
	// initialize vars
	int row;
	int col;
	int index;
	int isWinner = -1;
	char *boardPtr = (char*)malloc(sizeof(char));
	boardPtr = boardState;
	
	// inner loop only checks first four rows 
	// no horizontal win is possible in last three rows
	for(row = 0; row < BRD_ROWS; row++){
		for(col = 0; col < BRD_COLS - 3; col++){
			index = BRD_COLS * row + col;
			if(*boardPtr != ' ')
			{
				isWinner = compareSpaces(boardState, index, index + 1,
										  index + 2, index + 3);
			}
			if(*boardPtr == 'O' && isWinner == 1)
				isWinner++;
			
			boardPtr++;
			if(isWinner > 0)
				break;
		}
		boardPtr = boardPtr + 4;
		if(isWinner > 0)
				break;
	}
	return isWinner;
}

/*
	Loops through bottom half of the board looking for potential
	vertical straights. a potential straight is found when any character is found, 
	and then it sends that character with the next four spots in the array to the 
	compareSpaces() function. 
*/
int verticalWin(char *boardState)
{
	int row;
	int col;
	int index;
	int isWinner = -1;
	char *boardPtr = (char*)malloc(sizeof(char));
	boardPtr = boardState;
	
	// only loops through bottom 4 rows because no
	// vertical winner is possible within last three rows
	for(row = 0; row < BRD_ROWS - 3; row++){
		for(col = 0; col < BRD_COLS; col++){
			index = BRD_COLS * row + col;
			if(*boardPtr != ' ')
			{
				isWinner = compareSpaces(boardState, index, index + BRD_COLS,
										  index + BRD_COLS * 2, index + BRD_COLS * 3);
			}
			if(*boardPtr == 'O' && isWinner == 1)
				isWinner++;
			boardPtr++;
			if(isWinner > 0)
				break;
		}
		boardPtr++;
		if(isWinner > 0)
				break;
	}

	return isWinner;
}

/*
	loops through only the bottom-left corner of board
	looking for potential diaganol straights. 
*/
int diagonalWin(char *boardState)
{
	int row;
	int col;
	int index;
	int rghtDiag = 8; // equal to the number of spaces between a given piece
					  // and the next square in the right diaganol
	int lftDiag = 6;  // equal to the number of spaces between a given piece
					  // and the next square in the left diaganol
	int isWinner = -1;
	char *boardPtr = (char*)malloc(sizeof(char));
	boardPtr = boardState;

	for(row = 0; row < BRD_ROWS - 3; row++){
		for(col = 0; col < BRD_COLS; col++){
			index = BRD_COLS * row + col;
			boardPtr = boardState + index;
			if(*boardPtr != ' ')
			{	// checks right diaganol			
				if(col < BRD_COLS - 3)
				isWinner = compareSpaces(boardState, index, index + rghtDiag,
										  index + rghtDiag * 2, index + rghtDiag * 3);
				// checks left diaganol if right diagonal is not a winner				  
				if(isWinner == 0 && col >= 3){
					isWinner = compareSpaces(boardState, index, index + lftDiag,
										  index + lftDiag * 2, index + lftDiag * 3);
				}

			}
			if(*boardPtr == 'O' && isWinner == 1)
				isWinner++;
			
			boardPtr++;
			// break out of loop if winner is found
			if(isWinner > 0)
				break;
		}
		if(isWinner > 0)
				break;
		boardPtr++;
	}

	return isWinner;
}
/*
	Displays the board in a bottom-up fashion
*/
void displayWorld(char *bState)
{
	int indx = 1;
    char *tempBoardPtr = (char *)malloc(sizeof(char));
    tempBoardPtr = bState;
	printf("\e[1;1H\e[2J"); // clears screen
	printf("  | A | B | C | D | E | F | G |\n");

	for(int i = 5; i >-1; i--)
	{
		indx = (i*BRD_COLS);
		tempBoardPtr = bState +indx;
		printf("---------------------------------\n");
		printf("%d |", i + 1);
		for(int j = 0; j < 7; j++)
		{
			printf(" %c |", *tempBoardPtr );
			indx++;
			tempBoardPtr++;
		}
		printf("\n");
	}
	printf("---------------------------------\n");
	
}

