#include "parser.h"
#include "Utils/stack.h"

#define GRAMMAR_FILE "data/grammar.txt"

void    populateNtToEnum();
void    populateTkToEnum();
Grammar loadGrammarFromFile(char *grammarFile);
void    appendSymbolNode(ProductionRule *rule, SymbolNode *sn);
char   *getNonTerimnalName(NonTerminal nt);

static Grammar g;
hashtable      NtToEnum;
hashtable      TkToEnum;

// for debugging
// TODO surround in ifdef DEBUG_FLAG block
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

Grammar initParser(char *grammarFile) {
    ht_init(&NtToEnum, NONTERMINAL_COUNT);
    populateNtToEnum();

    ht_init(&TkToEnum, TOKEN_COUNT);
    populateTkToEnum();

    g = loadGrammarFromFile(grammarFile);
    return g;
}

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

    Grammar g = (Grammar){.num_nonterminals = NONTERMINAL_COUNT,
                          .start_symbol     = program, // FIXME: hardcoded
                          .derivations      = calloc(NONTERMINAL_COUNT, sizeof *g.derivations)};

    const int   buffsize = 1024;
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
                NonTerminal lhs = ht_lookup(NtToEnum, word);
                uint        rule_no;

                // new lhs, new set of derivations
                if (lhs != curr_lhs) {
                    curr_lhs = lhs;
                    rule_no  = 0;

                    g.derivations[lhs] = (Derivation){.lhs = lhs, .num_rhs = 1, .rhs = malloc(sizeof(ProductionRule))};
                }
                // make room for another production rule
                // A -> BC
                // A -> DE *(here)
                else {
                    rule_no = g.derivations[lhs].num_rhs;
                    g.derivations[lhs].num_rhs++;

                    g.derivations[lhs].rhs =
                        realloc(g.derivations[lhs].rhs, g.derivations[lhs].num_rhs * sizeof(ProductionRule));
                }
                g.derivations[lhs].rhs[rule_no] = (ProductionRule){.rule_length = 0, .tail = NULL, .head = NULL};
                continue;
            }

            // define the current symbol node
            TorNT tornt;
            int   type;
            uint  rule_no = g.derivations[curr_lhs].num_rhs - 1; // 0 indexed
            // NOTE: can cause problems
            if (word[0] == 'T') {
                type         = TYPE_TK;
                tornt.val_tk = ht_lookup(TkToEnum, word);
            } else {
                type         = TYPE_NT;
                tornt.val_nt = ht_lookup(NtToEnum, word);
            }
            // SymbolNode curr_sn = {.val = tornt, .type = type, .prev = NULL, .next = NULL};

            // in case pointer is reqd
            SymbolNode *curr_sn = malloc(sizeof *curr_sn);
            curr_sn->val        = tornt;
            curr_sn->type       = type;
            curr_sn->prev       = NULL;
            curr_sn->next       = NULL;

            // append the symbol node the current derivation's rhs
            appendSymbolNode(&g.derivations[curr_lhs].rhs[rule_no], curr_sn);
        }
    }

    free(buffer);

    fclose(fp);

    return g;
}

void populateNtToEnum() {
    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        ht_insert(&NtToEnum, getNonTerimnalName(i), i);
    }
}

void populateTkToEnum() {
    for (uint i = 0; i < TOKEN_COUNT; i++) {
        ht_insert(&TkToEnum, getTokenTypeName(i), i);
    }
}

void appendSymbolNode(ProductionRule *rule, SymbolNode *sn) {

    // first symbol in the rule
    if (rule->rule_length == 0) {
        rule->head = sn;
        rule->tail = sn;
        rule->rule_length++;
        return;
    }

    rule->tail->next = sn;
    sn->prev         = rule->tail;
    rule->tail       = sn;
    rule->rule_length++;
}

// recursively compute first sets
bitvector computeFirst(NonTerminal nt, FirstAndFollow *fnf, bool *first_computed) {
    if (first_computed[nt]) {
        return fnf[nt].first;
    }
    bv_init(&fnf[nt].first, TOKEN_COUNT);

    for (uint i = 0; i < g.derivations[nt].num_rhs; i++) {
        ProductionRule rule = g.derivations[nt].rhs[i];

        // epsilon production
        if (rule.rule_length == 0) {
            fnf[nt].has_eps = true;
            continue;
        }

        bool        more_symbols;
        SymbolNode *curr_symbol = rule.head;
        do {
            more_symbols = false;

            // stop if terminal
            if (curr_symbol->type == TYPE_TK) {
                bv_set(fnf[nt].first, curr_symbol->val.val_tk);
                continue;
            }

            // rule is of the form A -> BC..., nonterminal B

            // 1. recursively compute first(B)
            bitvector bv = computeFirst(curr_symbol->val.val_nt, fnf, first_computed);
            // 2. all elements in first(B) are in first(A)
            fnf[nt].first = bv_union(fnf[nt].first, bv, TOKEN_COUNT);
            // 3. if first(B) has eps, we need to add first(C) as well...
            if (fnf[curr_symbol->val.val_nt].has_eps)
                more_symbols = true;

            curr_symbol = curr_symbol->next;
        } while (curr_symbol != NULL && more_symbols);

        // 4. for A -> BCD, if we are still looking for more symbols
        // i.e. first(B), first(C), first(D) all have eps; first(A) has eps
        if (curr_symbol == NULL && more_symbols)
            fnf[nt].has_eps = true;
    }

    first_computed[nt] = true;
    return fnf[nt].first;
}

/**
 * Time Complexity: O(NUM_NONTERMINALS * NUM_TERMINALS^2 * NUM_PRODUCTION_RULES * max(NUM_NONTERMINALS_IN_RULE))
 * Actual running time will be much lower as most rules are short/many epsilon rules
 * Can be improved by early stopping - when the follow sets no longer change
 * But would require additional functionality to detect changes in the follow sets
 */
void computeFollow(FirstAndFollow *fnf) {

    // perform many rounds
    for (uint round = 0; round < NONTERMINAL_COUNT * TOKEN_COUNT + 1; round++) {
        for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
            for (uint j = 0; j < g.derivations[i].num_rhs; j++) {
                ProductionRule rule = g.derivations[i].rhs[j];

                // skip epsilon productions?
                if (rule.rule_length == 0)
                    continue;

                // A -> BCDE; follow(B) += first(C), follow(C) += first(D)...
                NonTerminal ntA = g.derivations[i].lhs;
                SymbolNode *t   = rule.head->next;
                while (t != NULL) {
                    if (t->prev->type == TYPE_TK) {
                        t = t->next;
                        continue;
                    }

                    NonTerminal ntB = t->prev->val.val_nt;
                    if (t->type == TYPE_TK) {
                        bv_set(fnf[ntB].follow, t->val.val_tk);
                    } else {
                        NonTerminal ntC = t->val.val_nt;
                        fnf[ntB].follow = bv_union(fnf[ntB].follow, fnf[ntC].first, TOKEN_COUNT);
                    }

                    // TODO CHECK
                    SymbolNode *t2 = t->next;
                    while (t2 != NULL) {
                        if (!fnf[t2->prev->val.val_nt].has_eps) {
                            break;
                        }

                        if (t2->type == TYPE_TK) {
                            bv_set(fnf[ntB].follow, t2->val.val_tk);
                            break;
                        }

                        NonTerminal ntD = t2->val.val_nt;
                        fnf[ntB].follow = bv_union(fnf[ntB].follow, fnf[ntD].first, TOKEN_COUNT);

                        t2 = t2->next;
                    }
                    t = t->next;
                }

                // A -> BCDE; if first(E) has eps, follow(D) += follow(A)
                // if first(E) and first(D) have eps, follow(C) += follow(A)
                // ...
                t = rule.tail->prev;
                while (t != NULL) {
                    if (t->type == TYPE_TK || t->next->type == TYPE_TK)
                        break;

                    NonTerminal ntD = t->val.val_nt;
                    if (!fnf[t->next->val.val_nt].has_eps)
                        break;

                    fnf[ntD].follow = bv_union(fnf[ntD].follow, fnf[ntA].follow, TOKEN_COUNT);

                    t = t->prev;
                }

                // A -> DBC; add all of follow(A) to follow(C)
                if (rule.tail->type == TYPE_NT) {
                    NonTerminal ntC = rule.tail->val.val_nt;
                    fnf[ntC].follow = bv_union(fnf[ntC].follow, fnf[ntA].follow, TOKEN_COUNT);
                }
            }
        }
    }
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
    bv_set(fnf[program].follow, TK_EOF); // FIXME hardcoded start symbol
    // iteratively compute follow in rounds
    computeFollow(fnf);

    return fnf;
}

void printGrammar(Grammar g) {
    for (int i = 0; i < NONTERMINAL_COUNT; i++) {
        Derivation curr_derivation = g.derivations[i];
        printf("\n%d. LHS: %s\n", i, NonTerminalNames[curr_derivation.lhs]);
        printf("RHS: %d rules\n", curr_derivation.num_rhs);

        for (int j = 0; j < curr_derivation.num_rhs; j++) {
            ProductionRule curr_rule = curr_derivation.rhs[j];
            SymbolNode    *trav      = curr_rule.head;
            printf("Rule Length: %d-> ", curr_rule.rule_length);
            if (curr_rule.rule_length == 0) {
                printf("epsilon");
            }
            while (trav != NULL) {
                if (trav->type == TYPE_TK) {
                    printf("%s ", getTokenTypeName(trav->val.val_tk));
                } else {
                    printf("%s ", getNonTerimnalName(trav->val.val_nt));
                }
                trav = trav->next;
            }
            printf("\n");
        }
    }

    printf("-----------------------------------------------------\n");
}

#define FNFOUTPUT_FILE "output/firstandfollow.txt"
void printFirstAndFollow(FirstAndFollow *fnf) {
    FILE *fp = fopen(FNFOUTPUT_FILE, "w");
    if (fp == NULL) {
        exit_msg("Could not open test case file");
    }

    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        fprintf(fp, "\nNon Terminal: %s\n", getNonTerimnalName(i));

        fprintf(fp, "First Set:  ");
        if (fnf[i].has_eps)
            fprintf(fp, "eps, ");
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

void fillPTCells(ParseTable pt, bitvector bv, ProductionRule rule, NonTerminal row) {
    for (uint i = 0; i < TOKEN_COUNT; i++) {
        if (!bv_contains(bv, i))
            continue;
        if (pt[row][i].filled) {
            printf("Duplicate entry at [%s, %s]\n", getNonTerimnalName(row), getTokenTypeName(i));
        }
        pt[row][i].filled = true;
        pt[row][i].rule   = rule;
    }
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
            if (rule.rule_length == 0) {
                fillPTCells(pt, fnf[lhs].follow, rule, lhs);
            } else {
                SymbolNode *t = rule.head;
                if (t->type == TYPE_TK) {
                    bitvector bv;
                    bv_init(&bv, TOKEN_COUNT);
                    bv_set(bv, rule.head->val.val_tk);
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

    return pt;
}

#define PTOUTPUT_FILE "output/parsetable.txt"
void printParseTable(ParseTable pt) {
    FILE *fp = fopen(PTOUTPUT_FILE, "w");
    if (fp == NULL) {
        exit_msg("Could not open test case file");
    }

    for (uint i = 0; i < NONTERMINAL_COUNT; i++) {
        for (uint j = 0; j < TOKEN_COUNT; j++) {
            fprintf(fp, "[%s, %s]: ", getNonTerimnalName(i), getTokenTypeName(j));
            if (!pt[i][j].filled) {
                fprintf(fp, "\n");
                continue;
            }

            fprintf(fp, "%s -> ", getNonTerimnalName(i));
            SymbolNode *trav = pt[i][j].rule.head;
            while (trav != NULL) {
                if (trav->type == TYPE_TK) {
                    fprintf(fp, "%s ", getTokenTypeName(trav->val.val_tk));
                } else {
                    fprintf(fp, "%s ", getNonTerimnalName(trav->val.val_nt));
                }
                trav = trav->next;
            }
            fprintf(fp, "\n");
        }
    }

    fclose(fp);
}

void parseInputSourceCode(char *testcaseFile) {
    FILE *fp = fopen(testcaseFile, "r");
    if (fp == NULL) {
        exit_msg("Could not open test case file");
    }

    // initialise lexer
    TwinBuffer tb = initLexer(fp);
    printf("Initialised lexer\n");

    // initialise parser
    Grammar g = initParser(GRAMMAR_FILE);
    printf("Loaded grammar\n");
    FirstAndFollow *fnf = computeFirstAndFollow(g);
    printf("Computed first and follow\n");
    printFirstAndFollow(fnf);
    ParseTable pt = createParseTable(fnf);
    printParseTable(pt);
    printf("Constructed Parse Table\n");

    // setup stack for parsing
    stack *s = calloc(1, sizeof(stack));
    st_push(s, TK_EOF, TYPE_TK);
    st_push(s, program, TYPE_NT); // start symbol

    // begin parsing
    TokenInfo t          = getNextToken(&tb);
    bool      has_errors = false;
    for (;;) {
        StackElement *se = st_top(s); // declare it outside?
        if (se == NULL)
            break;

        // redundant checks sometimes, can be optimized
        if (t.tk_type == END_TOKENTYPE) {
            printf("Lexical error found... stopping.\n");
            has_errors = true;
            break;
        }

        while (t.tk_type == TK_COMMENT) {
            freeToken(&t);
            t = getNextToken(&tb);
            continue;
        }

        /* if (se->type == TYPE_NT)
            printf("Top of the stack: %s. Current input token: %s. Current lexeme: %s\n", getNonTerimnalName(se->val),
                   getTokenTypeName(t.tk_type), t.lexeme);
        else
            printf("Top of the stack: %s. Current input token: %s. Current lexeme: %s\n", getTokenTypeName(se->val),
                   getTokenTypeName(t.tk_type), t.lexeme); */

        if (se->type == TYPE_TK) {
            if (se->val == t.tk_type) {
                st_pop(s); // stack and input match
                freeToken(&t);
                t = getNextToken(&tb);
            } else {
                printf("Syntax error: stack mismatch\n");
                freeToken(&t);
                t          = getNextToken(&tb);
                has_errors = true;
            }
        }

        else {
            NonTerminal    stack_val  = se->val;
            TokenType      curr_input = t.tk_type;
            ParseTableInfo pti        = pt[stack_val][curr_input];
            if (pti.filled) {
                st_pop(s);
                // fill in reverse order
                SymbolNode *t = pti.rule.tail;
                while (t != NULL) {
                    st_push(s, t->val.val_nt, t->type);
                    t = t->prev;
                }
            } else {
                // TODO: check if legal
                if (t.tk_type != TK_SEM) {
                    printf("Syntax error: parse table\n"); // TODO: error recovery
                    has_errors = true;
                }
                freeToken(&t);
                t = getNextToken(&tb);
            }
        }
    }

    if (!has_errors)
        printf("Input source code is syntatically correct\n");

    // free up resources
    freeParserData(&g, fnf, pt);
    freeTwinBuffer(&tb);
    st_free(s);
    free(s);
}

char *getNonTerimnalName(NonTerminal nt) { return NonTerminalNames[nt]; }

/****************** FREE UP ALL (C/M)ALLOCd ENTITIES ***********************/
void freeProductionRules(uint num_productions, ProductionRule *rule) {
    for (uint i = 0; i < num_productions; i++) {
        // free up the linked list of symbols
        SymbolNode *t = rule[i].head;
        while (t != NULL) {
            SymbolNode *tmp = t;

            t = t->next;
            free(tmp);
        }
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
