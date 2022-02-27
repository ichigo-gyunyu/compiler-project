#include "lexer.h"

#define BLOCK_SIZE 12

static FILE *fp; // pointer to the src file
static char  twin_buff[2][BLOCK_SIZE];
static uint  buff_size[2];
static int   curr_buff_no = 0;

static char *begin;   // pointer to the start of a lexeme
static char *forward; // lookahead pointer

static uint line_number = 1;

// for debugging
// TODO surround in ifdef DEBUG_FLAG block
static char *const TokenTypeNames[] = {
    [TK_ASSIGNOP]   = "TK_ASSIGNOP",
    [TK_COMMENT]    = "TK_COMMENT",
    [TK_FIELDID]    = "TK_FIELDID",
    [TK_ID]         = "TK_ID",
    [TK_NUM]        = "TK_NUM",
    [TK_RNUM]       = "TK_RNUM",
    [TK_FUNID]      = "TK_FUNID",
    [TK_RUID]       = "TK_RUID",
    [TK_WITH]       = "TK_WITH",
    [TK_PARAMETERS] = "TK_PARAMETERS",
    [TK_END]        = "TK_END",
    [TK_WHILE]      = "TK_WHILE",
    [TK_UNION]      = "TK_UNION",
    [TK_ENDUNION]   = "TK_ENDUNION",
    [TK_DEFINETYPE] = "TK_DEFINETYPE",
    [TK_AS]         = "TK_AS",
    [TK_TYPE]       = "TK_TYPE",
    [TK_MAIN]       = "TK_MAIN",
    [TK_GLOBAL]     = "TK_GLOBAL",
    [TK_PARAMETER]  = "TK_PARAMETER",
    [TK_LIST]       = "TK_LIST",
    [TK_SQL]        = "TK_SQL",
    [TK_SQR]        = "TK_SQR",
    [TK_INPUT]      = "TK_INPUT",
    [TK_OUTPUT]     = "TK_OUTPUT",
    [TK_INT]        = "TK_INT",
    [TK_REAL]       = "TK_REAL",
    [TK_COMMA]      = "TK_COMMA",
    [TK_SEM]        = "TK_SEM",
    [TK_COLON]      = "TK_COLON",
    [TK_DOT]        = "TK_DOT",
    [TK_ENDWHILE]   = "TK_ENDWHILE",
    [TK_OP]         = "TK_OP",
    [TK_CL]         = "TK_CL",
    [TK_IF]         = "TK_IF",
    [TK_THEN]       = "TK_THEN",
    [TK_ENDIF]      = "TK_ENDIF",
    [TK_READ]       = "TK_READ",
    [TK_WRITE]      = "TK_WRITE",
    [TK_RETURN]     = "TK_RETURN",
    [TK_PLUS]       = "TK_PLUS",
    [TK_MINUS]      = "TK_MINUS",
    [TK_MUL]        = "TK_MUL",
    [TK_DIV]        = "TK_DIV",
    [TK_CALL]       = "TK_CALL",
    [TK_RECORD]     = "TK_RECORD",
    [TK_ENDRECORD]  = "TK_ENDRECORD",
    [TK_ELSE]       = "TK_ELSE",
    [TK_AND]        = "TK_AND",
    [TK_OR]         = "TK_OR",
    [TK_NOT]        = "TK_NOT",
    [TK_LT]         = "TK_LT",
    [TK_LE]         = "TK_LE",
    [TK_EQ]         = "TK_EQ",
    [TK_GT]         = "TK_GT",
    [TK_GE]         = "TK_GE",
    [TK_NE]         = "TK_NE",
    [TK_EOF]        = "TK_EOF",
};

uint initBuffer(int buff_no);
char nextChar();
void retract();

void initLexer(FILE *src_ptr) {
    fp = src_ptr;

    // initialize the twin buffer
    initBuffer(curr_buff_no);
    begin   = twin_buff[curr_buff_no];
    forward = twin_buff[curr_buff_no];
}

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

char *getTokenTypeName(TokenType tk) { return TokenTypeNames[tk]; }

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
void removeComments(char *testcaseFile, char *cleanFile) {}
