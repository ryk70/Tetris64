# Makefile for Tetris64
# cc65: C code --> 6502asm
# ca65: 6502asm --> object files
# ld65: Links object files

CC = cc65
AS = ca65
LD = ld65
CFLAGS = -v -t c64

SRC = src
ASM = asm
BLD = build
OBJ = build/obj

compile:  $(OBJ)/main.o
	$(LD) $(CFLAGS) -o $(BLD)/tetris64 $(OBJ)/main.o c64.lib

$(OBJ)/main.o: $(ASM)/main.s
	$(AS) $(CFLAGS) -o $(OBJ)/main.o $^

$(ASM)/main.s: $(SRC)/main.c
	$(CC) $(CFLAGS) -O -o $(ASM)/main.s $^

.PHONY: clean

clean:
	rm -f $(OBJ)/*.o 
	rm -f $(ASM)/*.s