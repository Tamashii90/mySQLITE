#include "types.h"
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

void executeInsert(char *input) {
	int argc = 0;
	int id = 0;
	char *username = malloc(32);
	char *email = malloc(255);
	argc = sscanf(input, "insert %d %s %s", &id, username, email);
	if (argc < 3) puts("Invalid insert syntax.");

}

void executeSelect(char *input) {
	puts("Do select..");
}

void parseSqlCmd(char *input) {
	if (!strncmp(input, "insert", 6)) {
		executeInsert(input);
	} else if (!strncmp(input, "select", 6)) {
		executeSelect(input);
	} else {
		printf("Unrecognized SQL command '%s'\n", input);
	}
}
