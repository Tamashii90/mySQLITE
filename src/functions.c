#include "../include/types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* readInput() {
    size_t size = 1024; // Any value that fits the max size of all inputs
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
        closeDb(table);
        free(input);
        exit(0);
    } else {
        printf("Unrecognized command '%s'\n", input);
    }
}

Table *openDb(const char *fileName) {
    Pager *pager = openPager(fileName);
    Table *table = malloc(sizeof(Table));
    size_t numOfRows = pager->fileLength / ROW_SIZE;

    table->pager = pager;
    table->numOfRows = numOfRows;
    return table;
}

Pager *openPager(const char *fileName) {
    Pager *pager = malloc(sizeof(Pager));
    FILE *fd = fopen(fileName, "rb+");
    if (fd == NULL) {
        puts("Error opening file.");
        exit(1);
    }
    fseek(fd, 0, SEEK_END);
    pager->fd = fd;
    pager->fileLength = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    for (size_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

void closeDb(Table *table) {
    Pager *pager = table->pager;
    // This won't work when all pages are empty
    // size_t numOfFullPages = pager->fileLength / PAGE_SIZE;
    // size_t remainingPages = pager->fileLength % PAGE_SIZE;
    size_t numOfFullPages = table->numOfRows / ROWS_PER_PAGE;
    size_t remainingPages = table->numOfRows % ROWS_PER_PAGE;
    for (size_t i = 0; i < numOfFullPages; i++) {
        void *page = pager->pages[i];
        if (page == NULL) {
            continue;
        }
        flushPage(pager, i, PAGE_SIZE);
        free(page);
        pager->pages[i] = NULL;
    }
    // If the last page is partial page and it's not empty
    // then also write it to disk
    if (remainingPages > 0 && pager->pages[numOfFullPages] != NULL) {
        flushPage(pager, numOfFullPages, remainingPages * ROW_SIZE);
        free(pager->pages[numOfFullPages]);
        pager->pages[numOfFullPages] = NULL;
    }

    if (fclose(pager->fd) != 0) {
        puts("Error closing db..");
        exit(1);
    }

    free(pager);
    free(table);
}

void flushPage(Pager *pager, size_t pageNum, size_t pageSize) {
    if (pager->pages[pageNum] == NULL) {
        puts("Error: can't flush null page!");
        exit(1);
    }

    if (fseek(pager->fd, pageNum * pageSize, SEEK_SET) != 0) {
        puts("Can't seek for some reason..");
        exit(1);
    }

    if (fwrite(pager->pages[pageNum], pageSize, 1, pager->fd) != 1) {
        puts("Can't write.");
        exit(1);
    }
}

void *getRowAddress(Table *table, size_t rowNum) {
    size_t pageNum = rowNum / ROWS_PER_PAGE;
    void *page = getPage(table->pager, pageNum);
    size_t rowOffset = rowNum % ROWS_PER_PAGE;
    size_t byteOffset = rowOffset * ROW_SIZE;
    return page + byteOffset;
}

void *getPage(Pager *pager, size_t pageNum) {
    if (pageNum > TABLE_MAX_PAGES) {
        puts("Page out of bounds..");
        exit(1);
    }

    if (pager->pages[pageNum] == NULL) {
        pager->pages[pageNum] = malloc(PAGE_SIZE);
        FILE *fd = pager->fd;
        size_t pagesOnDisk = pager->fileLength / PAGE_SIZE;
        // A whole page might not fit at the end of the file
        if (pager->fileLength % PAGE_SIZE != 0) {
            pagesOnDisk++;
        }
        if (pageNum <= pagesOnDisk) {
            fseek(fd, pageNum * PAGE_SIZE, SEEK_SET);
            fread(pager->pages[pageNum], PAGE_SIZE, 1, fd);
            if (ferror(fd) != 0) {
                puts("Error reading file..");
                exit(1);
            }
        }
    }
    return pager->pages[pageNum];
}

void serializeRow(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &source->id, ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void executeInsert(Table *table, char *statement) {
    int argc = 0;
    short inputId = 0; // max value of short is 2^15 - 1 = 32767
    char username[USERNAME_SIZE];
    char email[EMAIL_SIZE];

    argc = sscanf_s(statement, "insert %hd %s %s", &inputId, username, USERNAME_SIZE, email, EMAIL_SIZE);
    if (argc < 3) puts("Invalid insert syntax.");
    else if (inputId <= 0) puts("Error: id must be from 1 to 32767.");
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
    puts("Returned rows:");
    for (size_t i = 0; i < table->numOfRows; i++) {
        deserializeRow(getRowAddress(table, i), &row);
        printf("%9s | %d | %s | %s |\n", "", row.id, row.username, row.email);
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