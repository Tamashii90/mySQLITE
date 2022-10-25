#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include "../include/constants.h"


typedef struct Row {
    short id;
    char username[21];
    char email[256];
} Row;

typedef struct Table {
    size_t numOfRows;
    void *pages[TABLE_MAX_PAGES];
} Table;


typedef enum StatementType {
    INSERT, SELECT
} StatementType;

typedef struct Statement {
    StatementType type;
    Row rowToInsert;
} Statement;


Table *newTable();
char* readInput();
void parseMetaCmd(Table *, char *);
void parseSqlCmd(Table *, char *);
void executeSelect(Table *, char *);
void executeInsert(Table *, char *);


#endif