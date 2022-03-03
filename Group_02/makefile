CC = gcc
CFLAGS = -O2
# CFLAGS = -Wall -g -fsanitize=address # for debugging
INC = $(wildcard *.h)
EXE = $(wildcard *.exe)
OBJ = $(SRC:.c=.o)
OUT = stage1exe
SRC = $(wildcard *.c)
TXT = $(wildcard output_*.txt)

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT) -lm
	@echo -e "\nDone. Usage: ./$(OUT) testCasefile.txt parseTreeOutFile.txt"

compile: $(SRC) $(INC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	rm -rf $(EXE) $(OUT) $(OBJ) $(TXT)
