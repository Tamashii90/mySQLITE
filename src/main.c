#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../include/types.h"

int main(void) {
    char *inputStr = NULL;
    Table *table = newTable();

    while (true) {
        printf("mySQLITE> ");
        free(inputStr);
        inputStr = readInput();
        if (inputStr == NULL) {
            puts("Something went wrong..");
        }

        if (inputStr[0] == '.') parseMetaCmd(table, inputStr);
        else parseSqlCmd(table, inputStr);
    }
}


