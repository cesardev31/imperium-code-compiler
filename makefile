CC = g++
CFLAGS = -std=c++17 -Wall -Wextra
TARGET = ImperiumCodeCompiler

all: $(TARGET)

$(TARGET): main.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) batalla.emperor batalla.cpp

.PHONY: all clean run