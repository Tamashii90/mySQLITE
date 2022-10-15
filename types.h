#include <stddef.h>
#ifndef TYPES_H
#define TYPES_H

typedef struct RowT {
    int id;
    char username[23];
    char email[255];
} Row;


typedef struct TableT {
    size_t numOfRows;
    void *pages[100];
} Table;


typedef enum StatementTypeT {
    INSERT, SELECT
} StatementType;

typedef struct StatementT {
    StatementType type;
    Row rowToInsert;
} Statement;

extern const size_t ID_SIZE;
extern const size_t USERNAME_SIZE;
extern const size_t EMAIL_SIZE;
extern const size_t ID_OFFSET;
extern const size_t USERNAME_OFFSET;
extern const size_t EMAIL_OFFSET;

char* readInput();
void parseMetaCmd(char *);
void parseSqlCmd(char *);
void executeSelect(char *);
void executeInsert(char *);


#endif