
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "lexer.h"

/********************************** DATA DEFINITIONS **********************************/

uint              line_number = 1;
Hashtable        *lookup_table;
bool              seen_eof = false;
static Hashtable *valid_chars; // char -> int
static bool       htinit = false;

// useful for printing relevant information
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

// may be used for optimizing time, tradeoff is memory
static char *const valid_starting_characters[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
    "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "_", "#", "<", "%",
    "[", "]", ",", ";", ",", ".", "(", ")", "+", "-", "*", "/", "&", "@", "~", "=", ">", "!",
};

/********************************** HELPER FUNCTION DECLERATIONS **********************************/

// note - not visible outside the lexer
char      nextChar(char **lex);
void      retract();
void      initLookupTable();
void      initValidCharsTable();
void      updateLexeme(char **lex, char c);
void      retractLexeme(char **lex);
void      lexemeToValue(TokenInfo *t);
TokenType getTokenFromKeyword(char *lex);
TokenType getMainOrFunID(char *lex);
TokenInfo accept(TwinBuffer *tb, TokenType t);
TokenInfo accept2(TwinBuffer *tb);
TokenInfo accept6(TwinBuffer *tb);
TokenInfo accept19(TwinBuffer *tb);
TokenInfo accept31(TwinBuffer *tb);
TokenInfo accept_noretract(TwinBuffer *tb, TokenType t);

/********************************** API FUNCTION DEFINITIONS **********************************/

// entry point for the lexical analyzer
TwinBuffer *initLexer(FILE *src_ptr) {
    line_number = 1;
    seen_eof    = false;

    // setup the twin buffer
    TwinBuffer *tb = tb_init(src_ptr);
    tb_loadNextBuff(tb);

    // initialize some hashtables
    if (!htinit) {
        initLookupTable();
        initValidCharsTable();
    }

    htinit = true;
    return tb;
}

// simulate the DFA to get tokens
#define ERROR_STATE 58
TokenInfo getNextToken(TwinBuffer *tb) {
    uint dfa_state        = 0;
    bool err_from_state_0 = false;

    char c;

    for (;;) {
        switch (dfa_state) {
        case 0:
            c = tb_nextChar(tb, &line_number);
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
            case '\r':
            case ' ':
            case '\n':
            case '\t':
                tb_resetBegin(tb);
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
                err_from_state_0 = true;
                dfa_state        = ERROR_STATE;
                break;
            }
            break;

        case 1:
            c = tb_nextChar(tb, &line_number);
            if (c >= 'a' && c <= 'z')
                dfa_state = 1;
            else
                dfa_state = 2;
            break;

        case 2:
            return accept2(tb);

        case 3:
            c = tb_nextChar(tb, &line_number);
            if (c >= 'a' && c <= 'z')
                dfa_state = 1;
            else if (c >= '2' && c <= '7')
                dfa_state = 4;
            else
                dfa_state = 2;
            break;

        case 4:
            c = tb_nextChar(tb, &line_number);
            if (c >= '2' && c <= '7')
                dfa_state = 5;
            else if (c >= 'b' && c <= 'd')
                dfa_state = 4;
            else
                dfa_state = 6;
            break;

        case 5:
            c = tb_nextChar(tb, &line_number);
            if (c >= '2' && c <= '7')
                dfa_state = 5;
            else
                dfa_state = 6;
            break;

        case 6:
            return accept6(tb);

        case 7:
            c = tb_nextChar(tb, &line_number);
            if (c >= '0' && c <= '9')
                dfa_state = 7;
            else if (c == '.')
                dfa_state = 9;
            else
                dfa_state = 8;
            break;

        case 8: {
            TokenInfo tf = accept(tb, TK_NUM);
            lexemeToValue(&tf);
            return tf;
        }

        case 9:
            c = tb_nextChar(tb, &line_number);
            if (c >= '0' && c <= '9')
                dfa_state = 10;
            else
                dfa_state = ERROR_STATE;
            break;

        case 10:
            c = tb_nextChar(tb, &line_number);
            if (c >= '0' && c <= '9')
                dfa_state = 11;
            else
                dfa_state = ERROR_STATE;
            break;

        case 11:
            c = tb_nextChar(tb, &line_number);
            if (c == 'E')
                dfa_state = 12;
            else
                dfa_state = 16;
            break;

        case 12:
            c = tb_nextChar(tb, &line_number);
            if (c == '+' || c == '-')
                dfa_state = 13;
            else if (c >= '0' && c <= '9')
                dfa_state = 14;
            else
                dfa_state = ERROR_STATE;
            break;

        case 13:
            c = tb_nextChar(tb, &line_number);
            if (c >= '0' && c <= '9')
                dfa_state = 14;
            else
                dfa_state = ERROR_STATE;
            break;

        case 14:
            c = tb_nextChar(tb, &line_number);
            if (c >= '0' && c <= '9')
                dfa_state = 15;
            else
                dfa_state = ERROR_STATE;
            break;

        case 15: {
            TokenInfo tf = accept_noretract(tb, TK_RNUM);
            lexemeToValue(&tf);
            return tf;
        }

        case 16: {
            TokenInfo tf = accept(tb, TK_RNUM);
            lexemeToValue(&tf);
            return tf;
        }

        case 17:
            c = tb_nextChar(tb, &line_number);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                dfa_state = 18;
            else
                dfa_state = ERROR_STATE;
            break;

        case 18:
            c = tb_nextChar(tb, &line_number);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                dfa_state = 18;
            else if (c >= '0' && c <= '9')
                dfa_state = 20;
            else
                dfa_state = 19;
            break;

        case 19:
            return accept19(tb);

        case 20:
            c = tb_nextChar(tb, &line_number);
            if (c >= '0' && c <= '9')
                dfa_state = 20;
            else
                dfa_state = 19;
            break;

        case 21:
            c = tb_nextChar(tb, &line_number);
            if (c >= 'a' && c <= 'z')
                dfa_state = 22;
            else
                dfa_state = ERROR_STATE;
            break;

        case 22:
            c = tb_nextChar(tb, &line_number);
            if (c >= 'a' && c <= 'z')
                dfa_state = 22;
            else
                dfa_state = 23;
            break;

        case 23:
            return accept(tb, TK_RUID);

        case 24:
            c = tb_nextChar(tb, &line_number);
            if (c == '-')
                dfa_state = 25;
            else if (c == '=')
                dfa_state = 28;
            else
                dfa_state = 29;
            break;

        case 25:
            c = tb_nextChar(tb, &line_number);
            if (c == '-')
                dfa_state = 26;
            else
                dfa_state = ERROR_STATE;
            break;

        case 26:
            c = tb_nextChar(tb, &line_number);
            if (c == '-')
                dfa_state = 27;
            else
                dfa_state = ERROR_STATE;
            break;

        case 27:
            return accept_noretract(tb, TK_ASSIGNOP);

        case 28:
            return accept_noretract(tb, TK_LE);

        case 29:
            return accept(tb, TK_LT);

        case 30:
            c = tb_nextChar(tb, &line_number);
            if (c == '\n')
                dfa_state = 31;
            else
                dfa_state = 30;
            break;

        case 31:
            return accept31(tb); // NOTE: retracting the \n here

        case 32:
            return accept_noretract(tb, TK_SQL);

        case 33:
            return accept_noretract(tb, TK_SQR);

        case 34:
            return accept_noretract(tb, TK_COMMA);

        case 35:
            return accept_noretract(tb, TK_SEM);

        case 36:
            return accept_noretract(tb, TK_COLON);

        case 37:
            return accept_noretract(tb, TK_DOT);

        case 38:
            return accept_noretract(tb, TK_OP);

        case 39:
            return accept_noretract(tb, TK_CL);

        case 40:
            return accept_noretract(tb, TK_PLUS);

        case 41:
            return accept_noretract(tb, TK_MINUS);

        case 42:
            return accept_noretract(tb, TK_MUL);

        case 43:
            return accept_noretract(tb, TK_DIV);

        case 44:
            c = tb_nextChar(tb, &line_number);
            if (c == '&')
                dfa_state = 45;
            else
                dfa_state = ERROR_STATE;
            break;

        case 45:
            c = tb_nextChar(tb, &line_number);
            if (c == '&')
                dfa_state = 46;
            else
                dfa_state = ERROR_STATE;
            break;

        case 46:
            return accept_noretract(tb, TK_AND);

        case 47:
            c = tb_nextChar(tb, &line_number);
            if (c == '@')
                dfa_state = 48;
            else
                dfa_state = ERROR_STATE;
            break;

        case 48:
            c = tb_nextChar(tb, &line_number);
            if (c == '@')
                dfa_state = 49;
            else
                dfa_state = ERROR_STATE;
            break;

        case 49:
            return accept_noretract(tb, TK_OR);

        case 50:
            return accept_noretract(tb, TK_NOT);

        case 51:
            c = tb_nextChar(tb, &line_number);
            if (c == '=')
                dfa_state = 52;
            else
                dfa_state = ERROR_STATE;
            break;

        case 52:
            return accept_noretract(tb, TK_EQ);

        case 53:
            c = tb_nextChar(tb, &line_number);
            if (c == '=')
                dfa_state = 54;
            else
                dfa_state = 55;
            break;

        case 54:
            return accept_noretract(tb, TK_GE);

        case 55:
            return accept(tb, TK_GT);

        case 56:
            c = tb_nextChar(tb, &line_number);
            if (c == '=')
                dfa_state = 57;
            else
                dfa_state = ERROR_STATE;
            break;

        case 57:
            return accept_noretract(tb, TK_NE);

        case ERROR_STATE:
            if (seen_eof || c == EOF) {
                seen_eof = true;
                return (TokenInfo){.tk_type = TK_EOF, .lexeme = NULL, .line_no = line_number};
            }

            char *tmp = calloc(2, sizeof *tmp);
            tmp[0]    = c;
            if (err_from_state_0) {
                printf(YEL "Line %3d Error: Unknown symbol <%c>" RESET "\n", line_number, c);
                tb_moveBegin(tb);
                return (TokenInfo){.line_no = line_number, .lexeme = tmp, .tk_type = END_TOKENTYPE};
            }

            tb_retract(tb, &line_number);
            char *lex = tb_getLexeme(tb);
            printf(YEL "Line %3d Error: Unknown pattern <%s>" RESET "\n", line_number, lex);
            /* if (ht_lookup(valid_chars, tmp) == -1)
                tb_nextChar(tb, &line_number); */
            free(tmp);

            return (TokenInfo){.tk_type = END_TOKENTYPE, .lexeme = lex, .line_no = line_number};
        }
    }
}

// handled independently of other functions in lexer
void removeComments(char *testcaseFile, char *cleanFile) {
    char  c;
    FILE *f1, *f2;
    f1 = fopen(testcaseFile, "r");
    if (cleanFile != NULL)
        f2 = fopen(cleanFile, "w");
    else
        f2 = stdout;

    bool is_comment = false;
    do {

        c = getc(f1);
        if (c == '%') {
            is_comment = true;
        }
        if (c == '\n') {
            is_comment = false;
        }
        if (feof(f1)) {
            break;
        }
        if (!is_comment) {
            fprintf(f2, "%c", c);
        }

    } while (1);

    fclose(f1);
    if (cleanFile != NULL)
        fclose(f2);
}

// get name from the enumerated token type value
char *getTokenTypeName(TokenType tk) { return TokenTypeNames[tk]; }

void runLexerOnInputSourceCode(char *testcaseFile) {
    FILE       *fp = fopen(testcaseFile, "r");
    TwinBuffer *tb = initLexer(fp);
    TokenInfo   t  = getNextToken(tb);

    // read token stream
    while (t.tk_type != TK_EOF) {
        if (t.tk_type != END_TOKENTYPE)
            printTokenInfo(t);
        freeToken(&t);
        t = getNextToken(tb);
    }
    freeToken(&t);
    freeTwinBuffer(tb);

    return;
}

/********************************** HELPER FUNCTION DEFINITIONS **********************************/

// hashtable for valid characters
void initValidCharsTable() {

    valid_chars =
        ht_init(sizeof(char), sizeof(int), (ht_hash)ht_moduloHash, NULL, NULL, NULL, NULL, NULL, HT_START_SIZE);

    uint n = NUM_ELEM(valid_starting_characters);
    for (uint i = 0; i < n; i++) {
        ht_insert(&valid_chars, valid_starting_characters + i, &i);
    }
}

// hashtable for all keywords
void initLookupTable() {

    lookup_table = ht_init(sizeof(char *), sizeof(int), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                           (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    char     *keyword_name;
    TokenType keyword_token;
    keyword_name  = "with";
    keyword_token = TK_WITH;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "parameters";
    keyword_token = TK_PARAMETERS;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "end";
    keyword_token = TK_END;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "while";
    keyword_token = TK_WHILE;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "union";
    keyword_token = TK_UNION;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "endunion";
    keyword_token = TK_ENDUNION;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "definetype";
    keyword_token = TK_DEFINETYPE;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "as";
    keyword_token = TK_AS;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "type";
    keyword_token = TK_TYPE;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "_main";
    keyword_token = TK_MAIN;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "global";
    keyword_token = TK_GLOBAL;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "parameter";
    keyword_token = TK_PARAMETER;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "list";
    keyword_token = TK_LIST;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "input";
    keyword_token = TK_INPUT;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "output";
    keyword_token = TK_OUTPUT;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "int";
    keyword_token = TK_INT;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "real";
    keyword_token = TK_REAL;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "endwhile";
    keyword_token = TK_ENDWHILE;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "if";
    keyword_token = TK_IF;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "then";
    keyword_token = TK_THEN;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "endif";
    keyword_token = TK_ENDIF;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "read";
    keyword_token = TK_READ;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "write";
    keyword_token = TK_WRITE;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "return";
    keyword_token = TK_RETURN;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "call";
    keyword_token = TK_CALL;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "record";
    keyword_token = TK_RECORD;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "endrecord";
    keyword_token = TK_ENDRECORD;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
    keyword_name  = "else";
    keyword_token = TK_ELSE;
    ht_insert(&lookup_table, &keyword_name, &keyword_token);
}

TokenType getTokenFromKeyword(char *lex) {
    int *res = ht_lookup(lookup_table, &lex);
    if (res == NULL)
        return TK_FIELDID;
    return *res;
}

TokenType getMainOrFunID(char *lex) {
    int *res = ht_lookup(lookup_table, &lex);
    if (res && *res == TK_MAIN)
        return *res;
    return TK_FUNID;
}

// convert string lexeme to int or real
void lexemeToValue(TokenInfo *t) {
    size_t m = strlen(t->lexeme);
    char   tmp[TMP_BUFLEN];

    // integer
    if (t->tk_type == TK_NUM) {
        t->val.val_int = strtoll(t->lexeme, NULL, 10);
    }

    // real
    else if (t->tk_type == TK_RNUM) {
        double exp = (t->lexeme[m - 1] - '0') + 10 * (t->lexeme[m - 2] - '0');
        if (t->lexeme[m - 3] == 'E') {
            strncpy(tmp, t->lexeme, m - 3);
            tmp[m - 3]      = '\0';
            double mantissa = strtod(tmp, NULL);
            t->val.val_real = mantissa * pow(10, exp);
        } else if (t->lexeme[m - 4] == 'E') {
            strncpy(tmp, t->lexeme, m - 4);
            tmp[m - 4]      = '\0';
            double mantissa = strtod(tmp, NULL);
            exp             = (t->lexeme[m - 3] == '+') ? exp : -exp;
            t->val.val_real = mantissa * pow(10, exp);
        } else {
            t->val.val_real = strtod(t->lexeme, NULL);
        }
    }
}

/********************************** PRINTING FUNCTIONS FOR LOGGING **********************************/

void printTokenInfo(TokenInfo t) {
    printf("Line no. %3d\t", t.line_no);
    printf("Lexeme %-30s", t.lexeme);
    printf("Token %s\n", getTokenTypeName(t.tk_type));
}

/********************************** DFA ACCEPT STATE HELPERS **********************************/

TokenInfo accept(TwinBuffer *tb, TokenType t) {
    tb_retract(tb, &line_number);
    char *lex = tb_getLexeme(tb);
    return (TokenInfo){.lexeme = lex, .tk_type = t, .line_no = line_number};
}

TokenInfo accept2(TwinBuffer *tb) {
    tb_retract(tb, &line_number);
    char     *lex = tb_getLexeme(tb);
    TokenType t   = getTokenFromKeyword(lex);
    return (TokenInfo){.lexeme = lex, .tk_type = t, .line_no = line_number};
}

TokenInfo accept6(TwinBuffer *tb) {
    tb_retract(tb, &line_number);
    char     *lex = tb_getLexeme(tb);
    TokenType t   = TK_ID;

    if (t == TK_ID && strlen(lex) > ID_MAX_LENGTH) {
        printf(YEL "Line %3d Error: Variable identifier is longer than prescribed length of 20 characters" RESET "\n",
               line_number);
        return (TokenInfo){.lexeme = lex, .tk_type = END_TOKENTYPE, .line_no = line_number};
    }
    return (TokenInfo){.lexeme = lex, .tk_type = t, .line_no = line_number};
}

TokenInfo accept19(TwinBuffer *tb) {
    tb_retract(tb, &line_number);
    char     *lex = tb_getLexeme(tb);
    TokenType t   = getMainOrFunID(lex);

    if (t == TK_FUNID && strlen(lex) > FUNID_MAX_LENGTH) {
        printf(YEL "Line %3d Error: Function identifier is longer than prescribed length of 30 characters" RESET "\n",
               line_number);
        return (TokenInfo){.lexeme = lex, .tk_type = END_TOKENTYPE, .line_no = line_number};
    }
    return (TokenInfo){.lexeme = lex, .tk_type = t, .line_no = line_number};
}

TokenInfo accept31(TwinBuffer *tb) {
    tb_retract(tb, &line_number);
    return (TokenInfo){.lexeme = str_dup("%"), .tk_type = TK_COMMENT, .line_no = line_number};
}

TokenInfo accept_noretract(TwinBuffer *tb, TokenType t) {
    char *lex = tb_getLexeme(tb);
    return (TokenInfo){.lexeme = lex, .tk_type = t, .line_no = line_number};
}

/********************************** FREE UP ALL MALLOCd ENTITIES **********************************/

void freeToken(TokenInfo *t) {
    if (t) {
        if (t->lexeme) {
            free(t->lexeme);
        }
    }
}

void freeTwinBuffer(TwinBuffer *tb) {
    if (tb) {
        fclose(tb->fp);
        free(tb);
    }
}

/**************************************************************************************************/
