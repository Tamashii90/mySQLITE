#include <stddef.h>
#include "../include/constants.h"
#include "../include/types.h"

const size_t ID_SIZE = GET_SIZE(Row, id);
const size_t USERNAME_SIZE = GET_SIZE(Row, username);
const size_t EMAIL_SIZE = GET_SIZE(Row, email);
const size_t ID_OFFSET = 0;
const size_t USERNAME_OFFSET = ID_SIZE + ID_OFFSET;
const size_t EMAIL_OFFSET = USERNAME_SIZE + USERNAME_OFFSET;
const size_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const size_t PAGE_SIZE = 4096;
const size_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const size_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;