CC = gcc
# CFLAGS = -O2 -w
CFLAGS = -Wall -g -fsanitize=address
LDFLAGS = -lm
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

OBJS = lexer.o \
	  parser.o \
	  utils.o \
	  bitvector.o \
	  hashtable.o \
	  twinbuffer.o \
	  stack.o \
	  nary.o \
	  vector.o \
	  driver.o

TXT = $(wildcard output_*.txt)

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT) $(LDFLAGS)
	@echo -e "\nDone. Usage: ./$(OUT) testCasefile.txt parseTreeOutFile.txt"

lexer.o: $(wildcard src/Lexer/*)
	$(CC) $(CFLAGS) $(INC) -c src/Lexer/lexer.c

parser.o: $(wildcard src/Parser/*)
	$(CC) $(CFLAGS) $(INC) -c src/Parser/parser.c

utils.o: $(wildcard src/Utils/utils*)
	$(CC) $(CFLAGS) $(INC) -c src/Utils/utils.c

bitvector.o: $(wildcard src/Utils/bitvector*)
	$(CC) $(CFLAGS) $(INC) -c src/Utils/bitvector.c

hashtable.o: $(wildcard src/Utils/hashtable*)
	$(CC) $(CFLAGS) $(INC) -c src/Utils/hashtable.c

twinbuffer.o: $(wildcard src/Utils/twinbuffer*)
	$(CC) $(CFLAGS) $(INC) -c src/Utils/twinbuffer.c

stack.o: $(wildcard src/Utils/stack*)
	$(CC) $(CFLAGS) $(INC) -c src/Utils/stack.c

nary.o: $(wildcard src/Utils/nary*)
	$(CC) $(CFLAGS) $(INC) -c src/Utils/nary.c

vector.o: $(wildcard src/Utils/vector*)
	$(CC) $(CFLAGS) $(INC) -c src/Utils/vector.c

driver.o: src/driver.c
	$(CC) $(CFLAGS) $(INC) -c src/driver.c

clean:
	rm -rf $(EXE) $(OUT) $(OBJS) $(TXT)
