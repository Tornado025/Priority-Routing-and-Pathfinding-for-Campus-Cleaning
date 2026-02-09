CC = gcc
CFLAGS = -Wall -Iinclude -g -fPIC

ifeq ($(OS), Windows_NT)
	SHARED_EXT = .dll
	SHARED_FLAGS = -shared
else ifeq ($(shell uname), Darwin)
	SHARED_EXT = .dylib
	SHARED_FLAGS = -dynamiclib
else
	SHARED_EXT = .so
	SHARED_FLAGS = -shared
endif

TARGET = libcampus$(SHARED_EXT)

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

directories:
ifeq ($(OS), Windows_NT)
	if not exist build mkdir build
else
	mkdir -p build
endif

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(SHARED_FLAGS) -o $@ $^

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS), Windows_NT)
	if exist build rmdir /s /q build
	if exist $(TARGET) del $(TARGET)
else
	rm -rf build $(TARGET)
endif
