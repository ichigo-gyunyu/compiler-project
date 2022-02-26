CC = gcc
CFLAGS = -Wall -g
INC = -I./src
EXE = $(wildcard *.exe)
OBJ = driver.o lexer.o
OUT = $(wildcard *.out)
SRC = ./src/driver.c ./src/Lexer/lexer.c

all: compile
	$(CC) $(OBJ)
	./a.out

compile:
	$(CC) $(CFLAGS) $(INC) -c $(SRC)

clean:
	rm -rf $(EXE) $(OUT)
