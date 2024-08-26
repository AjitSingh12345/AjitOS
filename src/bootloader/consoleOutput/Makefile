BUILD_DIR?=build/
ASM?=nasm
ASMFLAGS?=-f obj
CC16?=/usr/bin/watcom/binl/wcc
CFLAGS16?=-4 -d3 -s -wx -ms -zl -zq # -oneatxzh 
LD16?=/usr/bin/watcom/binl/wlink
SRC_FOLDER=consoleOutput

SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/consoleOutput/c/%.obj, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/consoleOutput/asm/%.obj, $(SOURCES_ASM))

.PHONY: all consoleOutput clean always

all: consoleOutput

stage2: $(BUILD_DIR)/consoleOutput.bin

$(BUILD_DIR)/consoleOutput.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	$(LD16) NAME $(BUILD_DIR)/consoleOutput.bin FILE \{ $(OBJECTS_ASM) $(OBJECTS_C) \} OPTION MAP=$(BUILD_DIR)/consoleOutput.map @linker.lnk

$(BUILD_DIR)/consoleOutput/c/%.obj: %.c always
	$(CC16) $(CFLAGS16) -fo=$@ $<

$(BUILD_DIR)/consoleOutput/asm/%.obj: %.asm always
	$(ASM) $(ASMFLAGS) -o $@ $<

always:
	mkdir -p $(BUILD_DIR)/consoleOutput/c
	mkdir -p $(BUILD_DIR)/consoleOutput/asm

clean:
	rm -f $(BUILD_DIR)/consoleOutput.bin