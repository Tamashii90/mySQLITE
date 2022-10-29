#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../include/types.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Enter filename of the database.");
        exit(1);
    }
    char *inputStr = NULL;
    char *dbName = argv[1];
    Table *table = openDb(dbName);

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


