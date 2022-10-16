SRC_DIR        := ./src
SRCS           := $(wildcard $(SRC_DIR)/*.c)
TARGET         := main
CC             := gcc
OPTS           := -Wall
OBJ_DIR        := ./obj
OBJS           := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))


# --------------------------------------------------

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(OPTS) -c $< -o $@

run: $(TARGET)
	./$(TARGET).exe

clean:
	rm -f $(OBJS) $(TARGET)