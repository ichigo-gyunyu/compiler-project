
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "parser.h"
#include "Utils/stack.h"

/********************************** DATA DEFINITIONS **********************************/

#define GRAMMAR_FILE "data/grammar.txt"

static Grammar g;
Hashtable     *NtToEnum;
Hashtable     *TkToEnum;
static bool    htinit = false;

// useful for printing relevant information
static char *const NonTerminalNames[] = {
    [actualOrRedefined]         = "actualOrRedefined",
    [arithmeticExpression]      = "arithmeticExpression",
    [assignmentStmt]            = "assignmentStmt",
    [booleanExpression]         = "booleanExpression",
    [conditionalStmt]           = "conditionalStmt",
    [constructedDatatype]       = "constructedDatatype",
    [dataType]                  = "dataType",
    [declaration]               = "declaration",
    [declarations]              = "declarations",
    [definetypestmt]            = "definetypestmt",
    [elsePart]                  = "elsePart",
    [expPrime]                  = "expPrime",
    [factor]                    = "factor",
    [fieldDefinition]           = "fieldDefinition",
    [fieldDefinitions]          = "fieldDefinitions",
    [fieldType]                 = "fieldType",
    [funCallStmt]               = "funCallStmt",
    [function]                  = "function",
    [global_or_not]             = "global_or_not",
    [highPrecedenceOperators]   = "highPrecedenceOperators",
    [idList]                    = "idList",
    [input_par]                 = "input_par",
    [inputParameters]           = "inputParameters",
    [ioStmt]                    = "ioStmt",
    [iterativeStmt]             = "iterativeStmt",
    [logicalOp]                 = "logicalOp",
    [lowPrecedenceOperators]    = "lowPrecedenceOperators",
    [mainFunction]              = "mainFunction",
    [moreExpansions]            = "moreExpansions",
    [moreFields]                = "moreFields",
    [more_ids]                  = "more_ids",
    [oneExpansion]              = "oneExpansion",
    [optionalReturn]            = "optionalReturn",
    [option_single_constructed] = "option_single_constructed",
    [otherFunctions]            = "otherFunctions",
    [otherStmts]                = "otherStmts",
    [output_par]                = "output_par",
    [outputParameters]          = "outputParameters",
    [parameter_list]            = "parameter_list",
    [primitiveDatatype]         = "primitiveDatatype",
    [program]                   = "program",
    [recOrUnion]                = "recOrUnion",
    [relationalOp]              = "relationalOp",
    [remaining_list]            = "remaining_list",
    [returnStmt]                = "returnStmt",
    [singleOrRecId]             = "singleOrRecId",
    [stmt]                      = "stmt",
    [stmts]                     = "stmts",
    [term]                      = "term",
    [termPrime]                 = "termPrime",
    [typeDefinition]            = "typeDefinition",
    [typeDefinitions]           = "typeDefinitions",
    [var]                       = "var",
};

/********************************** HELPER FUNCTION DECLERATIONS **********************************/

void      populateNtToEnum();
void      populateTkToEnum();
Grammar   loadGrammarFromFile(char *grammarFile);
void      appendSymbolNode(ProductionRule *rule, SymbolNode *sn);
char     *getNonTerimnalName(NonTerminal nt);
Bitvector computeFirst(NonTerminal nt, FirstAndFollow *fnf, bool *first_computed);
void      computeFollow(FirstAndFollow *fnf);
void      fillPTCells(ParseTable pt, Bitvector bv, ProductionRule rule, NonTerminal row);

/********************************** API FUNCTION DEFINITIONS **********************************/

// entry point for the syntax analyzer
Grammar initParser(char *grammarFile) {

    // populate hashtables to reference terminals and nonterminals
    if (!htinit) {
        populateNtToEnum();
        populateTkToEnum();
    }

    htinit = true;

    g = loadGrammarFromFile(grammarFile);
    return g;
}

FirstAndFollow *computeFirstAndFollow(const Grammar g) {

    FirstAndFollow *fnf = calloc(NONTERMINAL_COUNT, sizeof(*fnf));

    // keep track of what all first sets have been computed
    bool first_computed[NONTERMINAL_COUNT];
    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        first_computed[i] = false;
    }

    // recursively compute first
    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        NonTerminal lhs = g.derivations[i].lhs;
        if (first_computed[lhs]) {
            continue;
        }

        computeFirst(lhs, fnf, first_computed);
    }

    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        bv_init(&fnf[i].follow, TOKEN_COUNT);
    }
    // add '$' to follow(start symbol)
    bv_set(fnf[program].follow, TK_EOF); // care, hardcoded start symbol

    // iteratively compute follow in rounds
    computeFollow(fnf);

    return fnf;
}

ParseTable createParseTable(FirstAndFollow *fnf) {

    // init the parse table
    ParseTable pt = calloc(NONTERMINAL_COUNT, sizeof(ParseTableInfo *));
    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        pt[i] = calloc(TOKEN_COUNT, sizeof *pt[i]);
    }

    // loop through all production rules
    for (uint i = 0; i < g.num_nonterminals; i++) {
        NonTerminal lhs = g.derivations[i].lhs;
        for (uint j = 0; j < g.derivations[i].num_rhs; j++) {
            ProductionRule rule = g.derivations[i].rhs[j];

            // if A -> eps, add this rule to [A, b] for all b in follow(A)
            if (rule.rule_length == 0) {
                fillPTCells(pt, fnf[lhs].follow, rule, lhs);
            }

            else {
                SymbolNode *t = vec_getAt(rule.symbols, 0);

                // if A -> BCDE, add this rule to [A, b] for all b in first(BCDE)
                if (t->type == TYPE_TK) {
                    Bitvector bv;
                    bv_init(&bv, TOKEN_COUNT);
                    bv_set(bv, t->val.val_tk);
                    fillPTCells(pt, bv, rule, lhs);
                    free(bv);
                } else {
                    fillPTCells(pt, fnf[t->val.val_nt].first, rule, lhs);
                    if (fnf[t->val.val_nt].has_eps)
                        fillPTCells(pt, fnf[t->val.val_nt].follow, rule, lhs);
                }
            }
        }
    }

    // syn flag for the follow set
    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        for (uint j = 0; j < TOKEN_COUNT; j++) {
            if (bv_contains(fnf[i].follow, j)) {
                pt[i][j].syn = true;
            }
        }
    }

    return pt;
}

Nary_tree parseInputSourceCode(char *testcaseFile) {
    FILE *fp = fopen(testcaseFile, "r");
    if (fp == NULL) {
        exit_msg("Could not open test case file");
    }

    // initialise lexer
    TwinBuffer *tb = initLexer(fp);
    printf("\nInitialized lexer\n");

    // initialise parser
    Grammar g = initParser(GRAMMAR_FILE);
    printf("Loaded grammar\n");
    FirstAndFollow *fnf = computeFirstAndFollow(g);
    printf("Computed first and follow\n");
    printFirstAndFollow(fnf);
    ParseTable pt = createParseTable(fnf);
    printParseTable(pt);
    printf("Constructed Parse Table\n\n");

    // setup stack for parsing
    Stack *s = st_init(sizeof(SymbolNode), NULL, NULL);

    // push $
    st_push(s, &(SymbolNode){
                   .type       = TYPE_TK,
                   .val.val_tk = TK_EOF,
               });

    // push start symbol
    st_push(s, &(SymbolNode){
                   .type       = TYPE_NT,
                   .val.val_nt = program,
               });

    // start building the parse tree
    Nary_tree parsetree = nary_newNode((TokenInfo){.line_no = 1}, str_dup("program"), false);
    TreeNode *tn        = parsetree; // keeps track of the current node

    // begin parsing
    TokenInfo t          = getNextToken(tb);
    bool      has_errors = false;
    for (;;) {
        // empty stack
        SymbolNode *se = st_top(s); // declare it outside?
        if (se == NULL) {
            break;
        }

        // redundant checks sometimes, can be optimized
        if (t.tk_type == END_TOKENTYPE) {
            freeToken(&t);
            t = getNextToken(tb);

            has_errors = true;
            continue;
        }

        // ignore comments
        while (t.tk_type == TK_COMMENT) {
            freeToken(&t);
            t = getNextToken(tb);
            continue;
        }

#ifdef LOGGING
        if (se->type == TYPE_NT)
            printf("Top of the stack: %s. Current input token: %s. Current lexeme: %s\n", getNonTerimnalName(se->val),
                   getTokenTypeName(t.tk_type), t.lexeme);
        else
            printf("Top of the stack: %s. Current input token: %s. Current lexeme: %s\n", getTokenTypeName(se->val),
                   getTokenTypeName(t.tk_type), t.lexeme);
#endif

        // top of the stack is a token
        if (se->type == TYPE_TK) {
            if (se->val.val_tk == t.tk_type) {
                st_pop(s); // stack and input match

                if (t.tk_type == TK_EOF)
                    break;

                // update parse tree
                // nary_addChild(tn, t.tk_type, true, duplicate_str(t.lexeme), t.line_no, duplicate_str("----"));
                if (!has_errors) {
                    tn->t_info        = t;
                    tn->t_info.lexeme = str_dup(t.lexeme);
                    tn->is_leaf       = true;
                    free(tn->node_symbol);
                    tn->node_symbol = str_dup("----");

                    while (tn && !tn->next_sibling)
                        tn = tn->parent;
                    if (tn)
                        tn = tn->next_sibling;
                }

                freeToken(&t);
                t = getNextToken(tb);
            } else {
                has_errors = true;
                printf(RED "Line %3d Error: The token %s for lexeme %s does not match with the expected token %s" RESET
                           "\n",
                       t.line_no, getTokenTypeName(t.tk_type), t.lexeme, getTokenTypeName(se->val.val_tk));
                st_pop(s);

                // update parse tree node pointer
                if (!has_errors) {
                    while (tn && !tn->next_sibling)
                        tn = tn->parent;
                    if (tn)
                        tn = tn->next_sibling;
                }

                freeToken(&t);
                t = getNextToken(tb);

                // required?
                if (t.tk_type == TK_EOF)
                    break;
            }
        }

        // top of the stack is a nonterminal
        else {
            NonTerminal    stack_val  = se->val.val_nt;
            TokenType      curr_input = t.tk_type;
            ParseTableInfo pti        = pt[stack_val][curr_input];

            // corresponding entry exsits in parse table
            if (pti.filled) {
                st_pop(s);

                // fill production rule symbols in reverse order
                Vector *symbols = pti.rule.symbols;
                for (int i = symbols->size - 1; i >= 0; i--)
                    st_push(s, vec_getAt(symbols, i));

                if (!has_errors) {

                    if (pti.rule.rule_length == 0) {
                        nary_addChild(tn, (TokenInfo){.line_no = t.line_no, .lexeme = str_dup("----")},
                                      str_dup("epsilon"), true);
                    }

                    // populate the tree with the symbols of production rule
                    for (int i = 0; i < symbols->size; i++) {
                        SymbolNode *curr_symbol = vec_getAt(symbols, i);

                        char *tmp = (curr_symbol->type == TYPE_NT)
                                        ? str_dup(getNonTerimnalName(curr_symbol->val.val_nt))
                                        : str_dup("----");

                        nary_addChild(tn, (TokenInfo){.line_no = t.line_no}, tmp, false);
                    }

                    // move current tree node pointer
                    if (pti.rule.rule_length != 0)
                        tn = tn->first_child;
                    else {
                        while (tn && !tn->next_sibling)
                            tn = tn->parent;
                        if (tn)
                            tn = tn->next_sibling;
                    }
                }
            }

            // no corresponding entry in parse table
            else {
                // TODO: check if legal (ignore extra semicolons)
                if (t.tk_type != TK_SEM) {
                    printf(RED "Line %3d Error: Invalid token %s encountered with value %s stack top %s" RESET "\n",
                           t.line_no, getTokenTypeName(t.tk_type), t.lexeme, getNonTerimnalName(se->val.val_nt));
                    has_errors = true;

                    // panic mode
                    // recovery strategy - ignore input till syn symbol is read
                    bool has_syn = false;
                    do {
                        freeToken(&t);
                        t = getNextToken(tb);
                        if (t.tk_type == END_TOKENTYPE || t.tk_type == TK_EOF) {
                            break;
                        }
                        if (pt[stack_val][t.tk_type].syn)
                            has_syn = true;
                    } while (has_syn);

                    // found syn
                    if (has_syn) {
                        st_pop(s);

                        if (!has_errors) {
                            // update parse tree node pointer
                            while (tn && !tn->next_sibling)
                                tn = tn->parent;
                            if (tn)
                                tn = tn->next_sibling;
                        }
                    }
                }

                // ignore semicolon
                else {
                    freeToken(&t);
                    t = getNextToken(tb);
                }
            }
        }

        // if eof stop parsing
        if (t.tk_type == TK_EOF) {
            SymbolNode *se = st_top(s);
            if (!(se && se->type == TYPE_TK && se->val.val_tk == TK_EOF)) {
                has_errors = true;
            }

            break;
        }
    }

    if (!has_errors)
        printf(GRN "Input source code is syntatically correct" RESET "\n");
    else
        printf(RED "Syntax errors found" RESET "\n");

    if (!has_errors)
        printf("\nFirst and Follow sets, parse table and parse tree written to output files\n");

    // free up resources
    freeToken(&t);
    freeParserData(&g, fnf, pt);
    freeTwinBuffer(tb);
    st_free(s);

    if (!has_errors)
        return parsetree;

    nary_free(parsetree);
    return NULL;
}

// get name from the enumerated nonterminal value
char *getNonTerimnalName(NonTerminal nt) { return NonTerminalNames[nt]; }

/********************************** HELPER FUNCTION DEFINITIONS **********************************/

/**
 * NOTE: The grammar file must have nonterminals
 * with multiple derivations appear together. For eg.
 * A -> BCDE
 * A -> DEFG
 * B -> CCDE
 * B -> CBED
 *
 * 1024 is the max number of characters for a rule
 */
Grammar loadGrammarFromFile(char *grammarFile) {
    FILE *fp = fopen(grammarFile, "r");
    if (fp == NULL) {
        exit_msg("Could not open grammar file");
    }

    Grammar g = (Grammar){
        .num_nonterminals = NONTERMINAL_COUNT,
        .start_symbol     = program, // care, hardcoded
        .derivations      = calloc(NONTERMINAL_COUNT, sizeof *g.derivations),
    };

    const int   buffsize = TMP_BUFLEN;
    char       *buffer   = malloc(sizeof *buffer * buffsize);
    NonTerminal curr_lhs = -1;
    char       *word;

    // read a line
    while (fgets(buffer, buffsize, fp) != NULL) {
        char *line       = buffer;
        uint  words_read = 0;

        // get words from line
        while ((word = strtok_r(line, " ", &line)) != NULL) {
            if (word[0] == '-')
                continue;

            // remove trailing \n, if present
            uint len = strlen(word);
            if (word[len - 1] == '\n') {
                word[len - 1] = '\0';
            }

            words_read++;
            if (words_read == 1) {
                NonTerminal lhs =
                    *(NonTerminal *)ht_lookup(NtToEnum, &word); // NOTE: not checking for error in grammar.txt
                uint rule_no;

                // new lhs, new set of derivations
                if (lhs != curr_lhs) {
                    curr_lhs = lhs;
                    rule_no  = 0;

                    g.derivations[lhs] = (Derivation){.lhs = lhs, .num_rhs = 1, .rhs = malloc(sizeof(ProductionRule))};
                }
                // make room for another production rule
                // A -> BC
                // A -> DE <-- here
                else {
                    rule_no = g.derivations[lhs].num_rhs;
                    g.derivations[lhs].num_rhs++;

                    g.derivations[lhs].rhs =
                        realloc(g.derivations[lhs].rhs, g.derivations[lhs].num_rhs * sizeof(ProductionRule));
                }

                // initialize a new production rule (A -> BCDE)
                g.derivations[lhs].rhs[rule_no] = (ProductionRule){
                    .rule_length = 0,
                    .symbols     = vec_init(sizeof(SymbolNode), NULL, NULL, VEC_START_SIZE),
                };

                continue;
            }

            // define the current symbol node
            TorNT tornt;
            int   type;
            uint  rule_no = g.derivations[curr_lhs].num_rhs - 1; // 0 indexed
            // NOTE: assumes terminals start with T
            if (word[0] == 'T') {
                type         = TYPE_TK;
                tornt.val_tk = *(TokenType *)ht_lookup(TkToEnum, &word);
            } else {
                type         = TYPE_NT;
                tornt.val_nt = *(NonTerminal *)ht_lookup(NtToEnum, &word);
            }

            SymbolNode curr_sn = {
                .type = type,
                .val  = tornt,
            };

            // add this symbol node to the rhs vector
            vec_pushBack(g.derivations[curr_lhs].rhs[rule_no].symbols, &curr_sn);
            g.derivations[curr_lhs].rhs[rule_no].rule_length++;
        }
    }

    free(buffer);

    fclose(fp);

    return g;
}

// hashtable for nonterminals
void populateNtToEnum() {

    NtToEnum = ht_init(sizeof(char *), sizeof(int), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                       (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        char *nt = getNonTerimnalName(i);
        ht_insert(&NtToEnum, &nt, &i);
    }
}

// hashtable for terminals
void populateTkToEnum() {

    TkToEnum = ht_init(sizeof(char *), sizeof(int), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                       (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    for (uint i = 0; i < TOKEN_COUNT; i++) {
        char *t = getTokenTypeName(i);
        ht_insert(&TkToEnum, &t, &i);
    }
}

// recursively compute first sets for all nonterminals
Bitvector computeFirst(NonTerminal nt, FirstAndFollow *fnf, bool *first_computed) {

    // recursion base case
    if (first_computed[nt]) {
        return fnf[nt].first;
    }

    // initialize the bitvector to store the first set
    bv_init(&fnf[nt].first, TOKEN_COUNT);

    for (uint i = 0; i < g.derivations[nt].num_rhs; i++) {
        ProductionRule rule = g.derivations[nt].rhs[i];

        // epsilon production, add eps and continue
        if (rule.rule_length == 0) {
            fnf[nt].has_eps = true;
            continue;
        }

        // Traverse the symbols B, C, D... in A -> BCD...
        bool    more_symbols;
        Vector *symbols = rule.symbols;
        uint    i       = 0;

        SymbolNode *curr_symbol = vec_getAt(symbols, i++);
        do {
            more_symbols = false;

            // stop if terminal
            if (curr_symbol->type == TYPE_TK) {
                bv_set(fnf[nt].first, curr_symbol->val.val_tk);
                continue;
            }

            // rule is of the form A -> BC..., nonterminal B

            // 1. recursively compute first(B)
            Bitvector bv = computeFirst(curr_symbol->val.val_nt, fnf, first_computed);
            // 2. all elements in first(B) are in first(A)
            bv_union(fnf[nt].first, bv, TOKEN_COUNT);
            // 3. if first(B) has eps, we need to add first(C) as well...
            if (fnf[curr_symbol->val.val_nt].has_eps) {
                more_symbols = true;
            }

            curr_symbol = vec_getAt(symbols, i++);
        } while (i <= symbols->size && more_symbols);

        // 4. for A -> BCD, if we are still looking for more symbols
        // i.e. first(B), first(C), first(D) all have eps; first(A) has eps
        if (i > symbols->size && more_symbols)
            fnf[nt].has_eps = true;
    }

    first_computed[nt] = true;
    return fnf[nt].first;
}

/**
 * Time Complexity: O(NUM_NONTERMINALS * NUM_TERMINALS^2 * NUM_PRODUCTION_RULES * max(NUM_NONTERMINALS_IN_RULE^2))
 *
 * Actual running time will be much lower as most rules are short/many epsilon rules
 * Can be improved by early stopping - when the follow sets no longer change
 * But would require additional functionality to detect changes in the follow sets
 */
void computeFollow(FirstAndFollow *fnf) {

    // perform many rounds
    for (uint round = 0; round < NONTERMINAL_COUNT * TOKEN_COUNT + 1; round++) {

        // next 2 loops goes over each production rule
        for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
            for (uint j = 0; j < g.derivations[i].num_rhs; j++) {
                ProductionRule rule = g.derivations[i].rhs[j];

                // skip epsilon productions?
                if (rule.rule_length == 0)
                    continue;

                // case 1:
                // A -> BCDE; follow(B) += first(CDE), follow(C) += first(DE)...
                NonTerminal ntA     = g.derivations[i].lhs;
                Vector     *symbols = rule.symbols;           // the symbols B, C, D, E...
                int         ii      = 1;                      // C
                SymbolNode *t       = vec_getAt(symbols, ii); // current symbol
                while (ii < symbols->size) {
                    SymbolNode *tprev = vec_getAt(symbols, ii - 1);
                    if (tprev->type == TYPE_TK) {
                        t = vec_getAt(symbols, ++ii);
                        continue;
                    }

                    NonTerminal ntB = tprev->val.val_nt;
                    if (t->type == TYPE_TK) {
                        bv_set(fnf[ntB].follow, t->val.val_tk);
                    } else {
                        NonTerminal ntC = t->val.val_nt;
                        bv_union(fnf[ntB].follow, fnf[ntC].first, TOKEN_COUNT);
                    }

                    int         jj = ii + 1;
                    SymbolNode *t2 = vec_getAt(symbols, jj);
                    while (jj < symbols->size) {
                        SymbolNode *t2prev = vec_getAt(symbols, jj - 1);
                        if (!fnf[t2prev->val.val_nt].has_eps) {
                            break;
                        }

                        if (t2->type == TYPE_TK) {
                            bv_set(fnf[ntB].follow, t2->val.val_tk);
                            break;
                        }

                        NonTerminal ntD = t2->val.val_nt;
                        bv_union(fnf[ntB].follow, fnf[ntD].first, TOKEN_COUNT);

                        t2 = vec_getAt(symbols, ++jj);
                    }
                    t = vec_getAt(symbols, ++ii);
                }

                // case 2:
                // A -> BCDE; if first(E) has eps, follow(D) += follow(A)
                // if first(E) and first(D) have eps, follow(C) += follow(A)
                // ...
                ii = symbols->size - 2;
                t  = vec_getAt(symbols, ii);
                while (ii >= 0) {
                    SymbolNode *tnext = vec_getAt(symbols, ii + 1);
                    if (t->type == TYPE_TK || tnext->type == TYPE_TK)
                        break;

                    NonTerminal ntD = t->val.val_nt;
                    if (!fnf[tnext->val.val_nt].has_eps)
                        break;

                    bv_union(fnf[ntD].follow, fnf[ntA].follow, TOKEN_COUNT);

                    t = vec_getAt(symbols, --ii);
                }

                // case 3:
                // A -> DBC; add all of follow(A) to follow(C)
                SymbolNode *tail = vec_getAt(symbols, symbols->size - 1);
                if (tail->type == TYPE_NT) {
                    NonTerminal ntC = tail->val.val_nt;
                    bv_union(fnf[ntC].follow, fnf[ntA].follow, TOKEN_COUNT);
                }
            }
        }
    }
}

// helper function to fill an entry in the parsetable
void fillPTCells(ParseTable pt, Bitvector bv, ProductionRule rule, NonTerminal row) {
    for (uint i = 0; i < TOKEN_COUNT; i++) {
        if (!bv_contains(bv, i))
            continue;
        pt[row][i].filled = true;
        pt[row][i].rule   = rule;
    }
}

/********************************** PRINTING FUNCTIONS FOR LOGGING **********************************/

#define GROUTPUT_FILE "output_grammarverification.txt"
void printGrammar(Grammar g) {
    FILE *fp = fopen(GROUTPUT_FILE, "w");
    if (fp == NULL) {
        exit_msg("Could not open output file");
    }

    for (int i = 0; i < NONTERMINAL_COUNT; i++) {
        Derivation curr_derivation = g.derivations[i];
        fprintf(fp, "\n%d. LHS: %s\n", i, NonTerminalNames[curr_derivation.lhs]);
        fprintf(fp, "RHS: %d rules\n", curr_derivation.num_rhs);

        for (int j = 0; j < curr_derivation.num_rhs; j++) {
            ProductionRule curr_rule = curr_derivation.rhs[j];
            uint           ii        = 0;
            Vector        *symbols   = curr_rule.symbols;
            SymbolNode    *trav      = vec_getAt(symbols, ii);
            fprintf(fp, "Rule Length: %d-> ", curr_rule.rule_length);
            if (curr_rule.rule_length == 0) {
                fprintf(fp, "epsilon");
            }
            while (ii < symbols->size) {
                if (trav->type == TYPE_TK) {
                    fprintf(fp, "%s ", getTokenTypeName(trav->val.val_tk));
                } else {
                    fprintf(fp, "%s ", getNonTerimnalName(trav->val.val_nt));
                }
                trav = vec_getAt(symbols, ++ii);
            }
            fprintf(fp, "\n");
        }
    }
}

#define FNFOUTPUT_FILE "output_firstandfollow.txt"
void printFirstAndFollow(FirstAndFollow *fnf) {
    FILE *fp = fopen(FNFOUTPUT_FILE, "w");
    if (fp == NULL) {
        exit_msg("Could not open file to print first and follow");
    }

    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        fprintf(fp, "\nNon Terminal: %s\n", getNonTerimnalName(i));

        fprintf(fp, "First Set:  ");
        if (fnf[i].has_eps)
            fprintf(fp, "eps ");
        for (uint j = 0; j < TOKEN_COUNT; j++) {
            if (bv_contains(fnf[i].first, j)) {
                fprintf(fp, "%s ", getTokenTypeName(j));
            }
        }
        fprintf(fp, "\n");

        fprintf(fp, "Follow Set: ");
        for (uint j = 0; j < TOKEN_COUNT; j++) {
            if (bv_contains(fnf[i].follow, j)) {
                fprintf(fp, "%s ", getTokenTypeName(j));
            }
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

#define PTOUTPUT_FILE "output_parsetable.txt"
void printParseTable(ParseTable pt) {
    FILE *fp = fopen(PTOUTPUT_FILE, "w");
    if (fp == NULL) {
        exit_msg("Could not open file to print parse table");
    }

    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        for (uint j = 0; j < TOKEN_COUNT; j++) {
            fprintf(fp, "[%s, %s]: ", getNonTerimnalName(i), getTokenTypeName(j));
            if (!pt[i][j].filled) {
                fprintf(fp, "\n");
                continue;
            }

            fprintf(fp, "%s -> ", getNonTerimnalName(i));
            uint        ii      = 0;
            Vector     *symbols = pt[i][j].rule.symbols;
            SymbolNode *trav    = vec_getAt(symbols, ii);
            while (trav != NULL) {
                if (trav->type == TYPE_TK) {
                    fprintf(fp, "%s ", getTokenTypeName(trav->val.val_tk));
                } else {
                    fprintf(fp, "%s ", getNonTerimnalName(trav->val.val_nt));
                }
                trav = vec_getAt(symbols, ++ii);
            }
            fprintf(fp, "\n");
        }
    }

    fclose(fp);
}

Nary_tree printParseTree(Nary_tree pt, char *outfile) {
    if (!pt) {
        printf(RED "Not generating parse tree\n" RESET);
        return NULL;
    }
    FILE *fp = fopen(outfile, "w");
    if (!fp)
        exit_msg("Could not open parse tree output file\n");
    nary_printInorder(pt, &fp);
    fclose(fp);
    return pt;
}

/****************************** FREE UP ALL (C/M)ALLOCd ENTITIES **********************************/

void freeProductionRules(uint num_productions, ProductionRule *rule) {
    for (uint i = 0; i < num_productions; i++) {
        // free vector of symbols
        vec_free(rule[i].symbols);
    }
    free(rule);
}

void freeGrammarDerivations(uint num_derivations, Derivation *derivations) {
    for (uint i = 0; i < num_derivations; i++) {
        freeProductionRules(derivations[i].num_rhs, derivations[i].rhs);
    }
    free(derivations);
}

void freeGrammar(Grammar *g) { freeGrammarDerivations(g->num_nonterminals, g->derivations); }

void freeFnf(uint num_elems, FirstAndFollow *fnf) {
    for (uint i = 0; i < num_elems; i++) {
        free(fnf[i].first);
        free(fnf[i].follow);
    }
    free(fnf);
}

void freePT(ParseTable pt) {
    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        free(pt[i]);
    }
    free(pt);
}

void freeParserData(Grammar *g, FirstAndFollow *fnf, ParseTable pt) {
    if (fnf)
        freeFnf(g->num_nonterminals, fnf);
    if (g)
        freeGrammar(g);
    if (pt)
        freePT(pt);
}

/**************************************************************************************************/
