CC = gcc
CFLAGS = -Wall

TARGET = Daemon

# all: $(TARGET)

$(TARGET): main.o UtilityFunctions.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o UtilityFunctions.o

main.o: main.c
	$(CC) -c main.c

UtilityFunctions.o: UtilityFunctions.c UtilityFunctions.h
	$(CC) -c UtilityFunctions.c

clean:
	rm *.o