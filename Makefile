CFLAGS = -Wall -Wextra -std=c11 -pedantic
LIBS = 

lim: src/main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)