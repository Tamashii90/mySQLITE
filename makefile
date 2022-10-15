SRCS 		:= main.c functions.c
TARGET 		:= main
CC 			:= gcc
OPTS 		:= -Wall
OBJ_DIR		:= ./obj
OBJS 		:= $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))


# --------------------------------------------------

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(OPTS) -c $< -o $@

run: $(TARGET)
	./$(TARGET).exe

clean:
	rm -f $(OBJS) $(TARGET)