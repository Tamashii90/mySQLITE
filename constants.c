#include <stddef.h>
#include "types.h"
#define GET_SIZE(Struct, Att) sizeof(((Struct*)0)->Att)

const size_t ID_SIZE = GET_SIZE(Row, id);
const size_t USERNAME_SIZE = GET_SIZE(Row, username);
const size_t EMAIL_SIZE = GET_SIZE(Row, email);
const size_t ID_OFFSET = 0;
const size_t USERNAME_OFFSET = ID_SIZE + ID_OFFSET;
const size_t EMAIL_OFFSET = USERNAME_SIZE + USERNAME_OFFSET;