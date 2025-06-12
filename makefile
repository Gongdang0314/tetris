CC = gcc
SRC = tetris.c

# Detect OS
ifeq ($(OS),Windows_NT)
    TARGET = tetris.exe
    CFLAGS = -Wall -O2
    LDFLAGS = -lkernel32
else
    UNAME_S := $(shell uname -s)
    TARGET = tetris
    CFLAGS = -Wall -O2

    ifeq ($(UNAME_S),Darwin)  # macOS
        LDFLAGS =
    else ifeq ($(UNAME_S),Linux)
        LDFLAGS = -lm
    endif
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	if exist tetris.exe del /Q tetris.exe
else
	rm -f tetris tetris.exe
endif
