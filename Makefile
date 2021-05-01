CC = gcc
CFLAGS = -Wall

TARGET = Daemon

# all: $(TARGET)

$(TARGET): main.o UtilityFunctions.o Copy.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o UtilityFunctions.o Copy.o

main.o: main.c
	$(CC) -c main.c

UtilityFunctions.o: UtilityFunctions.c UtilityFunctions.h
	$(CC) -c UtilityFunctions.c

Copy.o: Copy.c Copy.h
	$(CC) -c Copy.c

clean:
	rm *.o