SRC:=src
BUILD:=build

CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
LIBS= 

$(BUILD)/lim: $(SRC)/main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	@rm -rf build/*

.PHONY: clean