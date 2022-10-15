#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "types.h"

int main(void) {
	char *inputStr = NULL;

	while (true) {
		printf("mySQLITE> ");
		free(inputStr);
		inputStr = readInput();
		if (inputStr == NULL) {
			puts("Something went wrong..");
		}

		if (inputStr[0] == '.') parseMetaCmd(inputStr);
		else parseSqlCmd(inputStr);
	}
}


