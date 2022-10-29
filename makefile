SRC_DIR        := ./src
SRCS           := $(wildcard $(SRC_DIR)/*.c)
TARGET         := main
CC             := gcc
OPTS           := -Wall
OBJ_DIR        := ./obj
OBJS           := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TEST_CP        := $(shell pwd):$(shell pwd)/include/junit-4.12.jar:$(shell pwd)/include/hamcrest-core-1.3.jar
TEST_CLASS     := TestRunner
DB_NAME        := DB.db
TEST_DB_NAME   := test.db


# --------------------------------------------------

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(OPTS) -c $< -o $@

$(TEST_CLASS).class : $(TEST_CLASS).java
	javac -cp $(TEST_CP) $(TEST_CLASS).java

run: $(TARGET)
	./$(TARGET) $(DB_NAME)

test: $(TARGET) $(TEST_CLASS).class
	rm -f $(TEST_DB_NAME)
	touch $(TEST_DB_NAME)
	java -cp $(TEST_CP) org.junit.runner.JUnitCore $(TEST_CLASS)

dbclean:
	rm $(DB_NAME)
	touch $(DB_NAME)

clean: dbclean
	rm -f $(OBJS) $(TARGET)