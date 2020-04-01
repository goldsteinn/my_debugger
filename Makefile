CC = gcc
CFLAGS = -O0 -g3 -D_GNU_SOURCE
LDFLAGS = -lpthread -lm

all: exe

exe:
	$(CC) $(CFLAGS) *.c -c $(LDFLAGS)
	$(CC) $(CFLAGS) *.o -o driver $(LDFLAGS)

clean:
	rm -f *.o *~ *#* driver
