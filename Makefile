CC = gcc
CFLAGS = -Wall -g -fsanitize=address
INC = -I./src
EXE = $(wildcard *.exe)
OUT = $(wildcard *.out)

# TODO use wildcards/pattern subst instead
SRC = ./src/Lexer/lexer.c \
./src/Parser/parser.c \
./src/Utils/utils.c \
./src/Utils/bitvector.c \
./src/Utils/hashtable.c \
./src/Utils/twinbuffer.c \
./src/Utils/stack.c \
 ./src/driver.c

OBJ = lexer.o \
	  parser.o \
	  utils.o \
	  bitvector.o \
	  hashtable.o \
	  twinbuffer.o \
	  stack.o \
	  driver.o

all: compile
	mkdir -p output
	$(CC) $(CFLAGS) $(OBJ)
	./a.out

compile:
	$(CC) $(CFLAGS) $(INC) -c $(SRC)

clean:
	rm -rf $(EXE) $(OUT) $(OBJ)
