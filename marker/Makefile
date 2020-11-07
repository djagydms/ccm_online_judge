BUILD_DIR=./build
SRC_DIR=./src
INC_DIR=./include
CC=gcc
OBJS=$(addprefix $(BUILD_DIR)/, main.o conf.o testcase.o c.o python.o)
TARGET=$(BUILD_DIR)/marker

all: $(TARGET)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) -I$(INC_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< -I$(INC_DIR)
