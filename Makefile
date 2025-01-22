SRC:=src
BUILD:=build
TEST:=tests

TARGET:= ./lasm ./lime ./delasm

CFLAGS=-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -std=c11 -pedantic
LIBS= 

all: $(BUILD)/lasm $(BUILD)/lime $(BUILD)/delasm
	@ln -s $(BUILD)/* .

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

examples: all $(TEST)/fib.lim $(TEST)/123.lim

$(TEST)/%.lim: $(TEST)/%.lasm
	./lasm $< $@

clean:
	@rm -rf $(BUILD) $(TEST)/*.lim $(TARGET)

.PHONY: all clean examples