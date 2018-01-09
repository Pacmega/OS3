#include <stdio.h>
#include "../structs.h"

number createNr(int value, char* pronunciation)
{
	number newNr;
	newNr.value = value;
	newNr.pronunciation = pronunciation;

	return newNr;
}

void createStructs(number numberArray[])
{
	numberArray[0] = createNr(1, "Ace");
	numberArray[1] = createNr(2, "Deuce");
	numberArray[2] = createNr(3, "Trey");
	numberArray[3] = createNr(4, "Cater");
	numberArray[4] = createNr(5, "Cinque");
	numberArray[5] = createNr(6, "Sice");
	numberArray[6] = createNr(7, "Seven");
	numberArray[7] = createNr(8, "Eight");
	numberArray[8] = createNr(9, "Nine");
}

int main(int argc, char const *argv[])
{
	printf("       Number test\n");
	printf("--------------------------\n");

	number numberArray[9];

	createStructs(numberArray);

	int i = 0;
	for(; i < 9; i++)
	{
		printf("%d - %s\n", numberArray[i].value, numberArray[i].pronunciation);
	}

	return 0;
}