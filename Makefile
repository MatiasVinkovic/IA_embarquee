CC = gcc
CFLAGS = -Wall -Wextra -Isrc
TARGET = test_step2

# fichiers 
SRCS = tests/test_step2.c src/perceptron.c src/read_pattern.c src/rng.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -lm

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)