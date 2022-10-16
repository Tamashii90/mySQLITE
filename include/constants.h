#ifndef CONSTANTS_H
#define CONSTANTS_H

#define TABLE_MAX_PAGES 100
#define GET_SIZE(Struct, Att) sizeof(((Struct*)0)->Att)

extern const size_t ID_SIZE;
extern const size_t USERNAME_SIZE;
extern const size_t EMAIL_SIZE;
extern const size_t ID_OFFSET;
extern const size_t USERNAME_OFFSET;
extern const size_t EMAIL_OFFSET;
extern const size_t ROW_SIZE;

extern const size_t PAGE_SIZE;
extern const size_t ROWS_PER_PAGE;
extern const size_t TABLE_MAX_ROWS;

#endif