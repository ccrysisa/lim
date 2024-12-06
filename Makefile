SRC:=src
BUILD:=build
TEST:=tests

CFLAGS=-Wall -Wextra -Wswitch-enum -std=c11 -pedantic
LIBS= 

all: $(BUILD)/lasm $(BUILD)/lime

$(BUILD)/lasm: $(SRC)/lim.h $(SRC)/lasm.c $(SRC)/lim.c
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)
	@ln -s $@

$(BUILD)/lime: $(SRC)/lim.h $(SRC)/lime.c $(SRC)/lim.c
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)
	@ln -s $@

examples: all $(TEST)/fib.lim 

$(TEST)/%.lim: $(TEST)/%.lasm
	./lasm $< $@

clean:
	@rm -rf ./lasm ./lime $(BUILD)/* $(TEST)/*.lim

.PHONY: all clean examples