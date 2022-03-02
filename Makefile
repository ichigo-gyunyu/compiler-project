CC = gcc
CFLAGS = -Wall
# CFLAGS = -Wall -g -fsanitize=address
INC = -I./src
EXE = $(wildcard *.exe)
OUT = stage1exe

# TODO use wildcards/pattern subst instead
SRC = ./src/Lexer/lexer.c \
./src/Parser/parser.c \
./src/Utils/utils.c \
./src/Utils/bitvector.c \
./src/Utils/hashtable.c \
./src/Utils/twinbuffer.c \
./src/Utils/stack.c \
./src/Utils/nary.c \
 ./src/driver.c

OBJ = lexer.o \
	  parser.o \
	  utils.o \
	  bitvector.o \
	  hashtable.o \
	  twinbuffer.o \
	  stack.o \
	  nary.o \
	  driver.o

TXT = output_firstandfollow.txt \
	  output_parsetable.txt \
	  output_pt.txt \
	  output_partsetree.txt

all: compile
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT) -lm
	@echo -e "\nDone. Usage: ./$(OUT) testCasefile.txt parseTreeOutFile.txt"

compile:
	$(CC) $(CFLAGS) $(INC) -c $(SRC)

clean:
	rm -rf $(EXE) $(OUT) $(OBJ) $(TXT)
