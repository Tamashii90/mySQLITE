#include "../include/types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* readInput() {
    size_t size = 255;
    char *input = malloc(size);
    char *res = fgets(input, size, stdin);
    if (res != NULL) {
        // Remove EOL character
        *(strchr(res, '\n')) = 0;
    }
    return res;
}

void parseMetaCmd(char *input) {
    if (!strcmp(input, ".exit")) {
        free(input);
        exit(0);
    } else {
        printf("Unrecognized command '%s'\n", input);
    }
}

Table *newTable() {
    Table *table = malloc(sizeof(Table));
    table->numOfRows = 0;
    for (size_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }
    return table;
}

void *getRowAddress(Table *table, size_t rowNum) {
    size_t pageNum = rowNum / ROWS_PER_PAGE;
    void *page = table->pages[pageNum];
    if (page == NULL) {
        page = table->pages[pageNum] = malloc(PAGE_SIZE);
    }
        size_t rowOffset = rowNum % ROWS_PER_PAGE;
        size_t byteOffset = rowOffset * ROW_SIZE;
        return page + byteOffset;
    return page;
}

void serializeRow(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &source->id, ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &source->username, USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &source->email, EMAIL_SIZE);
}

void executeInsert(Table *table, char *statement) {
    int argc = 0;
    Row row;
    argc = sscanf(statement, "insert %d %s %s", &(row.id), row.username, row.email);
    if (argc < 3) puts("Invalid insert syntax.");
    else if (table->numOfRows >= TABLE_MAX_ROWS) puts("Table is full..");
    else {
        void *rowAddress = getRowAddress(table, table->numOfRows);
        // printf("Row address: %p\n", rowAddress);
        serializeRow(&row, rowAddress);
        table->numOfRows++;
        printf("Inserted 1 row. Current rows: %zu\n", table->numOfRows);
    }
}

void deserializeRow(void *source, Row *destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(destination->username, source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(destination->email, source + EMAIL_OFFSET, EMAIL_SIZE);
}

void executeSelect(Table *table, char *input) {
    Row row;
    // This is more elegant than looping over each row in each page.
    for (size_t i = 0; i < table->numOfRows; i++) {
        deserializeRow(getRowAddress(table, i), &row);
        printf("%d %s %s\n", row.id, row.username, row.email);
    }
    // for (size_t i = 0; i < TABLE_MAX_PAGES; i++) {
    // 	void *page = table->pages[i];
    // 	if (page == NULL) continue;
    // 	for (size_t j = 0; j < ROWS_PER_PAGE; j++) {
    // 		// printf("Got address %p\n", page);
    // 		deserializeRow(page, &row);
    // 		if (row.username[0] != 0) printf("%d %s %s\n", row.id, row.username, row.email);
    // 		page += ROW_SIZE;
    // 	}
    // }
}

void parseSqlCmd(Table *table, char *input) {
    if (!strncmp(input, "insert", 6)) {
        executeInsert(table, input);
    } else if (!strcmp(input, "select")) {
        executeSelect(table, input);
    } else {
        printf("Unrecognized SQL command '%s'\n", input);
    }
}
