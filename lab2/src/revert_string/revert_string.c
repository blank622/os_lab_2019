#include "revert_string.h"
#include <string.h>
#include <stdlib.h>

void RevertString(char *str)
{
	char *reversed = malloc(sizeof(char) * strlen(str));
	for (int i = strlen(str)-1, j = 0; i >= 0; i--, j++)
	{
		reversed[j] = str[i];
	}

	strcpy(str, reversed);
}
