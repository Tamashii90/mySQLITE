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

void parseMetaCmd(Table *table, char *input) {
    if (!strcmp(input, ".exit")) {
        free(input);
        free(table);
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

void freeTable(Table *table) {
    for (size_t i = 0; table->pages[i] != NULL; i++) {
        free(table->pages[i]);
    }
    free(table);
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
    short inputId = 0;
    char username[USERNAME_SIZE];
    char email[EMAIL_SIZE];

    argc = sscanf_s(statement, "insert %hd %s %s", &inputId, username, USERNAME_SIZE - 1, email, EMAIL_SIZE - 1);
    if (argc < 3) puts("Invalid insert syntax.");
    else if (inputId < 0) puts("Error: id must be positive.");
    else if (table->numOfRows >= TABLE_MAX_ROWS) puts("Table is full..");
    else {
        Row row;
        row.id = inputId;
        strcpy(row.username, username);
        strcpy(row.email, email);
        void *rowAddress = getRowAddress(table, table->numOfRows);
        // printf("Row address: %p\n", rowAddress);
        serializeRow(&row, rowAddress);
        table->numOfRows++;
        printf("Row inserted. Current rows: %zu\n", table->numOfRows);
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
