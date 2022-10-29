#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include "../include/constants.h"


typedef struct Row {
    short id;
    char username[21];
    char email[256];
} Row;

typedef struct Pager {
    FILE *fd;
    size_t fileLength;
    void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct Table {
    size_t numOfRows;
    Pager *pager;
} Table;

typedef enum StatementType {
    INSERT, SELECT
} StatementType;

typedef struct Statement {
    StatementType type;
    Row rowToInsert;
} Statement;


Table *openDb(const char *fileName);
Pager *openPager(const char *fileName);
char* readInput();
void parseMetaCmd(Table *, char *);
void parseSqlCmd(Table *, char *);
void executeSelect(Table *, char *);
void executeInsert(Table *, char *);
void flushPage(Pager *, size_t, size_t);
void *getPage(Pager *, size_t);
void closeDb(Table *);


#endif