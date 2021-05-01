CC = gcc
CFLAGS = -Wall

TARGET = Daemon

all: $(TARGET)

$(TARGET): main.c UtilityFunctions.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c UtilityFunctions.c

clean:
	rm -f $(TARGET) *.o