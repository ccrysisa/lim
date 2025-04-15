SRC:=src
BUILD:=build
TEST:=tests

CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -O3 -std=c11 -pedantic
LIBS= 

all: $(BUILD)/lasm $(BUILD)/lime $(BUILD)/delasm

$(BUILD)/lasm: $(SRC)/lim.h $(SRC)/lim.c $(SRC)/lasm.c
	@if [ ! -d "$(dir $@)" ]; then mkdir -p $(BUILD); fi
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)

$(BUILD)/lime: $(SRC)/lim.h $(SRC)/lim.c $(SRC)/lime.c
	@if [ ! -d "$(dir $@)" ]; then mkdir -p $(BUILD); fi
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)

$(BUILD)/delasm: $(SRC)/lim.h $(SRC)/lim.c $(SRC)/delasm.c
	@if [ ! -d "$(dir $@)" ]; then mkdir -p $(BUILD); fi
	$(CC) $(CFLAGS) $(filter-out $<, $^) -o $@ $(LIBS)

$(BUILD)/nan: $(SRC)/nan.c
	@if [ ! -d "$(dir $@)" ]; then mkdir -p $(BUILD); fi
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

$(TEST)/%.lim: $(TEST)/%.lasm
	$(BUILD)/lasm -i $< -o $@

examples: all $(patsubst %.lasm, %.lim, $(wildcard $(TEST)/*.lasm))

clean:
	@rm -rf $(BUILD) $(TEST)/*.lim

.PHONY: all clean examples