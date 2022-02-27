#include "lexer.h"

#define BLOCK_SIZE 12

static FILE *fp; // pointer to the src file
static char  twin_buff[2][BLOCK_SIZE];
static uint  buff_size[2];
static int   curr_buff_no = 0;

static char *begin;   // pointer to the start of a lexeme
static char *forward; // lookahead pointer

static uint line_number       = 1;
static bool lexer_initialised = false;

hashtable lookup_table;

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
char nextChar(char **lex);
void retract();
void initLookupTable();
void updateLexeme(char **lex, char c);
void retractLexeme(char **lex);

void initLexer(FILE *src_ptr) {
    fp = src_ptr;

    // initialize the twin buffer
    initBuffer(curr_buff_no);
    begin   = twin_buff[curr_buff_no];
    forward = twin_buff[curr_buff_no];

    initLookupTable();

    lexer_initialised = true;
}

void initLookupTable() {
    ht_init(&lookup_table, KEYWORD_COUNT);

    // clang-format off
    ht_insert(&lookup_table, "with",        TK_WITH);
    ht_insert(&lookup_table, "parameters",  TK_PARAMETERS);
    ht_insert(&lookup_table, "end",         TK_END);
    ht_insert(&lookup_table, "while",       TK_WHILE);
    ht_insert(&lookup_table, "union",       TK_UNION);
    ht_insert(&lookup_table, "endunion",    TK_ENDUNION);
    ht_insert(&lookup_table, "definetype",  TK_DEFINETYPE);
    ht_insert(&lookup_table, "as",          TK_AS);
    ht_insert(&lookup_table, "type",        TK_TYPE);
    ht_insert(&lookup_table, "_main",       TK_MAIN);
    ht_insert(&lookup_table, "global",      TK_GLOBAL);
    ht_insert(&lookup_table, "parameter",   TK_PARAMETER);
    ht_insert(&lookup_table, "list",        TK_LIST);
    ht_insert(&lookup_table, "input",       TK_INPUT);
    ht_insert(&lookup_table, "output",      TK_OUTPUT);
    ht_insert(&lookup_table, "int",         TK_INT);
    ht_insert(&lookup_table, "real",        TK_REAL);
    ht_insert(&lookup_table, "endwhile",    TK_ENDWHILE);
    ht_insert(&lookup_table, "if",          TK_IF);
    ht_insert(&lookup_table, "then",        TK_THEN);
    ht_insert(&lookup_table, "endif",       TK_ENDIF);
    ht_insert(&lookup_table, "read",        TK_READ);
    ht_insert(&lookup_table, "write",       TK_WRITE);
    ht_insert(&lookup_table, "return",      TK_RETURN);
    ht_insert(&lookup_table, "call",        TK_CALL);
    ht_insert(&lookup_table, "record",      TK_RECORD);
    ht_insert(&lookup_table, "endrecord",   TK_ENDRECORD);
    ht_insert(&lookup_table, "else",        TK_ELSE);
    // clang-format on
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

char nextChar(char **lex) {

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
    updateLexeme(lex, c);
    return c;
}

void retract(char **lex) {
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

    retractLexeme(lex);
}

char *getTokenTypeName(TokenType tk) { return TokenTypeNames[tk]; }

// TODO: can make it more efficient
void updateLexeme(char **lex, char c) {
    int len       = strlen(*lex);
    *lex[len]     = c;
    *lex[len + 1] = '\0';
}

// TODO bounds checking
void retractLexeme(char **lex) {
    int len       = strlen(*lex);
    *lex[len - 1] = '\0';
}

TokenType getTokenFromKeyword(char *lex) {
    int res = ht_lookup(lookup_table, lex);
    if (res == -1)
        return TK_FIELDID;
    return res;
}

TokenType getMainOrFunID(char *lex) {
    int res = ht_lookup(lookup_table, lex);
    if (res == TK_MAIN)
        return res;
    return TK_FUNID;
}

#define ERROR_STATE 58
TokenInfo getNextToken() {
    // few things
    // 1. how to handle no more tokens (eof)?
    // 2. how to handle errors?
    // im thinking have a status variable that is passed as an out param
    // other way would be to modify TokenInfo to accomodate these
    // but that would likely cause more problems

    if (!lexer_initialised) {
        printf("Lexer has not been initialised\n");
        return (TokenInfo){.tk_type = END_TOKENTYPE};
    }

    char *lex = calloc(128, sizeof *lex); // care

    uint dfa_state = 0;

    char c = nextChar(&lex);
    for (;;) {
        switch (dfa_state) {
        case 0:
            switch (c) {
            case 'a':
                dfa_state = 1;
                break;
            case 'b':
            case 'c':
            case 'd':
                dfa_state = 3;
                break;
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
                dfa_state = 1;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                dfa_state = 7;
                break;
            case ' ':
            case '\n':
            case '\t':
                dfa_state = 0;
                break;
            case '_':
                dfa_state = 17;
                break;
            case '#':
                dfa_state = 21;
                break;
            case '<':
                dfa_state = 24;
                break;
            case '%':
                dfa_state = 30;
                break;
            case '[':
                dfa_state = 32;
                break;
            case ']':
                dfa_state = 33;
                break;
            case ',':
                dfa_state = 34;
                break;
            case ';':
                dfa_state = 35;
                break;
            case ':':
                dfa_state = 36;
                break;
            case '.':
                dfa_state = 37;
                break;
            case '(':
                dfa_state = 38;
                break;
            case ')':
                dfa_state = 39;
                break;
            case '+':
                dfa_state = 40;
                break;
            case '-':
                dfa_state = 41;
                break;
            case '*':
                dfa_state = 42;
                break;
            case '/':
                dfa_state = 43;
                break;
            case '&':
                dfa_state = 44;
                break;
            case '@':
                dfa_state = 47;
                break;
            case '~':
                dfa_state = 50;
                break;
            case '=':
                dfa_state = 51;
                break;
            case '>':
                dfa_state = 53;
                break;
            case '!':
                dfa_state = 56;
                break;
            default:
                dfa_state = ERROR_STATE;
                break;
            }
            break;

        case 1:
            c = nextChar(&lex);
            if (c < 'a' || c > 'z')
                dfa_state = 2;
            break;

        case 2:
            retract(&lex);
            TokenType t2 = getTokenFromKeyword(lex);
            // accept
            return (TokenInfo){.tk_type = t2, .lexeme = lex, .line_no = line_number};

        case 3:
            c = nextChar(&lex);
            if (c >= 'a' && c <= 'z')
                dfa_state = 1;
            else if (c >= '2' && c <= '7')
                dfa_state = 4;
            else
                dfa_state = 2;
            break;

        case 4:
            c = nextChar(&lex);
            if (c >= '2' && c <= '7')
                dfa_state = 5;
            else if (c >= 'b' && c <= 'd')
                dfa_state = 4;
            else
                dfa_state = 6;
            break;

        case 5:
            c = nextChar(&lex);
            if (c >= '2' && c <= '7')
                dfa_state = 5;
            else
                dfa_state = 6;
            break;

        case 6:
            retract(&lex);
            // accept
            return (TokenInfo){.line_no = line_number, .lexeme = lex, .tk_type = TK_ID};

        case 7:
            c = nextChar(&lex);
            if (c >= '0' && c <= '9')
                dfa_state = 7;
            else if (c == '.')
                dfa_state = 9;
            else
                dfa_state = 8;
            break;

        case 8:
            retract(&lex);
            // accept
            return (TokenInfo){.tk_type = TK_NUM, .lexeme = lex, .line_no = line_number};

        case 9:
            c = nextChar(&lex);
            if (c >= '0' && c <= '9')
                dfa_state = 10;
            else
                dfa_state = ERROR_STATE;
            break;

        case 10:
            c = nextChar(&lex);
            if (c >= '0' && c <= '9')
                dfa_state = 11;
            else
                dfa_state = ERROR_STATE;
            break;

        case 11:
            c = nextChar(&lex);
            if (c == 'E')
                dfa_state = 12;
            else
                dfa_state = 16;
            break;

        case 12:
            c = nextChar(&lex);
            if (c == '+' || c == '-')
                dfa_state = 13;
            else if (c >= '0' && c <= '9')
                dfa_state = 14;
            else
                dfa_state = ERROR_STATE;
            break;

        case 13:
            c = nextChar(&lex);
            if (c >= '0' && c <= '9')
                dfa_state = 14;
            else
                dfa_state = ERROR_STATE;
            break;

        case 14:
            c = nextChar(&lex);
            if (c >= '0' && c <= '9')
                dfa_state = 15;
            else
                dfa_state = ERROR_STATE;
            break;

        case 15:
            // accept
            return (TokenInfo){.line_no = line_number, .lexeme = lex, .tk_type = TK_RNUM};

        case 16:
            retract(&lex);
            // accept
            return (TokenInfo){.tk_type = TK_RNUM, .lexeme = lex, .line_no = line_number};

        case 17:
            c = nextChar(&lex);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                dfa_state = 18;
            else
                dfa_state = ERROR_STATE;
            break;

        case 18:
            c = nextChar(&lex);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                dfa_state = 18;
            else if (c >= '0' && c <= '9')
                dfa_state = 20;
            else
                dfa_state = 19;
            break;

        case 19:
            retract(&lex);
            TokenType t19 = getMainOrFunID(lex);
            // accept
            return (TokenInfo){.tk_type = t19, .lexeme = lex, .line_no = line_number};

        case 20:
            c = nextChar(&lex);
            if (c >= '0' && c <= '9')
                dfa_state = 20;
            else
                dfa_state = 19;
            break;

        case 21:
            c = nextChar(&lex);
            if (c <= 'a' && c >= 'z')
                dfa_state = 22;
            else
                dfa_state = ERROR_STATE;
            break;

        case 22:
            c = nextChar(&lex);
            if (c <= 'a' && c >= 'z')
                dfa_state = 22;
            else
                dfa_state = 23;
            break;

        case 23:
            retract(&lex);
            // accept
            return (TokenInfo){.line_no = line_number, .lexeme = lex, .tk_type = TK_RUID};

        case 24:
            c = nextChar(&lex);
            if (c == '-')
                dfa_state = 25;
            else if (c == '=')
                dfa_state = 28;
            else
                dfa_state = 29;
            break;

        case 25:
            c = nextChar(&lex);
            if (c == '-')
                dfa_state = 26;
            else
                dfa_state = ERROR_STATE;
            break;

        case 26:
            c = nextChar(&lex);
            if (c == '-')
                dfa_state = 27;
            else
                dfa_state = ERROR_STATE;
            break;

        case 27:
            // accept
            return (TokenInfo){.tk_type = TK_ASSIGNOP, .lexeme = lex, .line_no = line_number};

        case 28:
            // accept
            return (TokenInfo){.tk_type = TK_LE, .lexeme = lex, .line_no = line_number};

        case 29:
            retract(&lex);
            // accept
            return (TokenInfo){.line_no = line_number, .lexeme = lex, .tk_type = TK_LT};

        case 30:
            c = nextChar(&lex);
            if (c == '\n')
                dfa_state = 31;
            else
                dfa_state = 30;
            break;

        case 31:
            // accept
            return (TokenInfo){.tk_type = TK_COMMENT, .line_no = line_number, .lexeme = lex};

        case 32:
            // accept
            return (TokenInfo){.tk_type = TK_SQL, .line_no = line_number, .lexeme = lex};

        case 33:
            // accept
            return (TokenInfo){.tk_type = TK_SQR, .line_no = line_number, .lexeme = lex};

        case 34:
            // accept
            return (TokenInfo){.tk_type = TK_COMMA, .line_no = line_number, .lexeme = lex};

        case 35:
            // accept
            return (TokenInfo){.tk_type = TK_SEM, .line_no = line_number, .lexeme = lex};

        case 36:
            // accept
            return (TokenInfo){.tk_type = TK_COLON, .line_no = line_number, .lexeme = lex};

        case 37:
            // accept
            return (TokenInfo){.tk_type = TK_DOT, .line_no = line_number, .lexeme = lex};

        case 38:
            // accept
            return (TokenInfo){.tk_type = TK_OP, .line_no = line_number, .lexeme = lex};

        case 39:
            // accept
            return (TokenInfo){.tk_type = TK_CL, .line_no = line_number, .lexeme = lex};

        case 40:
            // accept
            return (TokenInfo){.tk_type = TK_PLUS, .line_no = line_number, .lexeme = lex};

        case 41:
            // accept
            return (TokenInfo){.tk_type = TK_MINUS, .line_no = line_number, .lexeme = lex};

        case 42:
            // accept
            return (TokenInfo){.tk_type = TK_MUL, .line_no = line_number, .lexeme = lex};

        case 43:
            // accept
            return (TokenInfo){.tk_type = TK_DIV, .line_no = line_number, .lexeme = lex};

        case 44:
            c = nextChar(&lex);
            if (c == '&')
                dfa_state = 45;
            else
                dfa_state = ERROR_STATE;
            break;

        case 45:
            c = nextChar(&lex);
            if (c == '&')
                dfa_state = 46;
            else
                dfa_state = ERROR_STATE;
            break;

        case 46:
            // accept
            return (TokenInfo){.tk_type = TK_AND, .line_no = line_number, .lexeme = lex};

        case 47:
            c = nextChar(&lex);
            if (c == '@')
                dfa_state = 48;
            else
                dfa_state = ERROR_STATE;
            break;

        case 48:
            c = nextChar(&lex);
            if (c == '@')
                dfa_state = 49;
            else
                dfa_state = ERROR_STATE;
            break;

        case 49:
            // accept
            return (TokenInfo){.tk_type = TK_OR, .line_no = line_number, .lexeme = lex};

        case 50:
            // accept
            return (TokenInfo){.tk_type = TK_NOT, .line_no = line_number, .lexeme = lex};

        case 51:
            c = nextChar(&lex);
            if (c == '=')
                dfa_state = 52;
            else
                dfa_state = ERROR_STATE;
            break;

        case 52:
            // accept
            return (TokenInfo){.tk_type = TK_EQ, .line_no = line_number, .lexeme = lex};

        case 53:
            c = nextChar(&lex);
            if (c == '=')
                dfa_state = 54;
            else
                dfa_state = 55;
            break;

        case 54:
            // accept
            return (TokenInfo){.tk_type = TK_GE, .line_no = line_number, .lexeme = lex};

        case 55:
            retract(&lex);
            // accept
            return (TokenInfo){.tk_type = TK_GT, .line_no = line_number, .lexeme = lex};

        case 56:
            c = nextChar(&lex);
            if (c == '=')
                dfa_state = 57;
            else
                dfa_state = ERROR_STATE;
            break;

        case 57:
            // accept
            return (TokenInfo){.tk_type = TK_GE, .line_no = line_number, .lexeme = lex};

        case ERROR_STATE:
            printf("error, lol\n"); // TODO error handling
            return (TokenInfo){.tk_type = END_TOKENTYPE};
        }
    }
}

// handled independently of other functions in lexer
// (just for demonstration probably)
void removeComments(char *testcaseFile, char *cleanFile) {}
