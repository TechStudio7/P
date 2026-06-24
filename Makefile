CC = gcc
CFLAGS = -O2 -std=c11

all: ptranspiler

ptranspiler: src/main.c
	$(CC) $(CFLAGS) -o ptranspiler src/main.c

clean:
	rm -f ptranspiler
