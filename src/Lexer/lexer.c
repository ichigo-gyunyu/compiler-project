#include "lexer.h"

#define BLOCK_SIZE 12


static FILE *fp; // pointer to the src file
static char  twin_buff[2][BLOCK_SIZE];
static uint  buff_size[2];
static int   curr_buff_no = 0;

static char *begin;   // pointer to the start of a lexeme
static char *forward; // lookahead pointer

static uint line_number = 1;

uint initBuffer(int buff_no);
char nextChar();
void retract();

uint initBuffer(int buff_no) {
    uint bytes_read = fread(twin_buff[buff_no], 1, BLOCK_SIZE, fp);
    if (ferror(fp)) {
        perror("Error when initializing buffer");
        exit(EXIT_FAILURE);
    }

    if (feof(fp)) {
        twin_buff[buff_no][bytes_read] = EOF;
    }

    buff_size[curr_buff_no] = bytes_read;

    return bytes_read;
}

void initLexer(FILE *src_ptr) {
    fp = src_ptr;

    // initialize the twin buffer
    initBuffer(curr_buff_no);
    begin   = twin_buff[curr_buff_no];
    forward = twin_buff[curr_buff_no];
}

char nextChar() {

    // check for buffer overflow
    if (forward - twin_buff[curr_buff_no] >= BLOCK_SIZE) {
        curr_buff_no = !curr_buff_no;
        initBuffer(curr_buff_no);
        forward = twin_buff[curr_buff_no];
    }

    char c = *forward;
    if (c == '\n')
        line_number++;
    forward++;
    return c;
}

void retract() {
    if (*forward == '\n')
        line_number--;

    // switch to the previous buffer in case of underflow
    // and move the file pointer back
    if (forward == twin_buff[curr_buff_no]) {
        fseek(fp, -buff_size[curr_buff_no], SEEK_CUR);
        curr_buff_no = !curr_buff_no;
        forward      = twin_buff[curr_buff_no] + buff_size[curr_buff_no] - 1;
    } else {
        forward--;
    }
}

TokenInfo getNextToken(int *status) {
    // few things
    // 1. how to handle no more tokens (eof)?
    // 2. how to handle errors?
    // im thinking have a status variable that is passed as an out param
    // other way would be to modify TokenInfo to accomodate these
    // but that would likely cause more problems
}

// handled independently of other functions in lexer
// (just for demonstration probably)
void removeComments(char *testcaseFile, char *cleanFile) {

}
