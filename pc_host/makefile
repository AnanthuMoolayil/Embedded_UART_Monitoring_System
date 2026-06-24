CC = clang

CFLAGS = -std=c17 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wshadow \
	-Wconversion \
	-Wsign-conversion \
	-Wformat=2 \
	-g \
	-fsanitize=address,undefined

LDLIBS = -lserialport

SRC_DIR = source_files

SRC = $(wildcard $(SRC_DIR)/*.c)

TARGET = pc_host

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

clean:
	rm -f $(TARGET)