CC = gcc
CFLAGS = -Wall -Wextra -Isrc
TARGET = test_step1

# fichiers 
SRCS = tests/test_step1.c src/perceptron.c src/read_pattern.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)