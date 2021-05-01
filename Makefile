CC = gcc
CFLAGS = -Wall

TARGET = Daemon

# all: $(TARGET)

Daemon: main.c UtilityFunctions.c
	$(CC) $(CFLAGS) -o Daemon main.c UtilityFunctions.c

clean:
	rm -f *.o