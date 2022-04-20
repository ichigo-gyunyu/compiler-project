CC = gcc
# CFLAGS = -O2 -w
# CFLAGS = -Wall -g -fsanitize=address
CFLAGS = -Wall -g
LDFLAGS = -lm
INC = -I./src
EXE = $(wildcard *.exe)
OUT = compiler

OBJS = lexer.o \
	  parser.o \
	  utils.o \
	  bitvector.o \
	  hashtable.o \
	  twinbuffer.o \
	  stack.o \
	  nary.o \
	  vector.o \
	  ast.o \
	  symbolTable.o \
	  typeChecker.o \
	  semantics.o \
	  codegen.o \
	  driver.o

TXT = $(wildcard output_*.txt)

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT) $(LDFLAGS)
	@echo -e "\nDone. Usage: ./$(OUT) testCasefile.txt code.asm"

lexer.o: $(wildcard src/Lexer/*)
	$(CC) $(CFLAGS) $(INC) -c src/Lexer/lexer.c

parser.o: $(wildcard src/Parser/*)
	$(CC) $(CFLAGS) $(INC) -c src/Parser/parser.c

ast.o: $(wildcard src/AST/*)
	$(CC) $(CFLAGS) $(INC) -c src/AST/ast.c

symbolTable.o: $(wildcard src/SymbolTable/*)
	$(CC) $(CFLAGS) $(INC) -c src/SymbolTable/symbolTable.c

typeChecker.o: $(wildcard src/TypeChecker/*)
	$(CC) $(CFLAGS) $(INC) -c src/TypeChecker/typeChecker.c

semantics.o: $(wildcard src/Semantics/*)
	$(CC) $(CFLAGS) $(INC) -c src/Semantics/semantics.c

codegen.o: $(wildcard src/CodeGen/*)
	$(CC) $(CFLAGS) $(INC) -c src/CodeGen/codegen.c

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

leakcheck: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all -s ./compiler testcases/s3.txt output_pt.txt
	@echo Remove address sanitizer before using valgrind

clean:
	rm -rf $(EXE) $(OUT) $(OBJS) $(TXT) a.out code.o
