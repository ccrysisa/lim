SRC:=src
BUILD:=build
TEST:=tests

TARGET:= ./lasm ./lime ./delasm

CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -std=c11 -pedantic
LIBS= 

all: $(BUILD)/lasm $(BUILD)/lime $(BUILD)/delasm

$(BUILD)/lasm: $(SRC)/lim.h $(SRC)/lim.c $(SRC)/lasm.c
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)
	@ln -s $@

$(BUILD)/lime: $(SRC)/lim.h $(SRC)/lim.c $(SRC)/lime.c
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)
	@ln -s $@

$(BUILD)/delasm: $(SRC)/lim.h $(SRC)/lim.c $(SRC)/delasm.c
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)
	@ln -s $@

examples: all $(TEST)/fib.lim $(TEST)/123.lim

$(TEST)/%.lim: $(TEST)/%.lasm
	./lasm $< $@

clean:
	@rm -rf $(BUILD)/* $(TEST)/*.lim $(TARGET)

.PHONY: all clean examples