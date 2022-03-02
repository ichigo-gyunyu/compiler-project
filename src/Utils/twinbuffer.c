
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "twinbuffer.h"

void tb_loadNextBuff(TwinBuffer *tb) {
    uint bytes_read = fread(tb->buff[tb->lookahead_buffnum], 1, BLOCKSZ, *(tb->fp));
    if (ferror(*tb->fp)) {
        perror("Error when initializing buffer");
        exit(EXIT_FAILURE);
    }

    if (feof(*(tb->fp))) {
        tb->buff[tb->lookahead_buffnum][bytes_read] = EOF;
    }

    tb->used[tb->lookahead_buffnum] = bytes_read;
}

char tb_nextChar(TwinBuffer *tb, uint *linenum) {
    char c = *(tb->lookahead);

    if (c == '\n')
        *linenum = *linenum + 1;
    tb->lookahead++;

    // check for buffer overflow
    if (tb->lookahead - tb->buff[tb->lookahead_buffnum] >= BLOCKSZ) {
        tb->lookahead_buffnum = !tb->lookahead_buffnum;
        tb_loadNextBuff(tb);
        tb->lookahead = tb->buff[tb->lookahead_buffnum];
    }

    return c;
}

void tb_retract(TwinBuffer *tb, uint *linenum) {
    // switch to the previous buffer in case of underflow
    // and move the file pointer back
    if (tb->lookahead == tb->buff[tb->lookahead_buffnum]) {
        long seek_back = (long)(tb->used[tb->lookahead_buffnum]) * -1;
        fseek(*(tb->fp), seek_back, SEEK_CUR);
        tb->lookahead_buffnum = !tb->lookahead_buffnum;
        tb->lookahead         = tb->buff[tb->lookahead_buffnum] + tb->used[tb->lookahead_buffnum] - 1;
    } else {
        tb->lookahead--;
    }

    if (tb->lookahead == tb->buff[tb->lookahead_buffnum] - 1)
        printf("XXX\n");

    if (*tb->lookahead == '\n')
        *linenum = *linenum - 1;
}

char tb_moveBegin(TwinBuffer *tb) {
    char c = *tb->begin;
    tb->begin++;

    // check for buffer overflow
    if (tb->begin - tb->buff[tb->begin_buffnum] >= BLOCKSZ) {
        tb->begin_buffnum = !tb->begin_buffnum;
        tb->begin         = tb->buff[tb->begin_buffnum];
    }

    return c;
}

char *tb_getLexeme(TwinBuffer *tb) {
    char *lex = calloc(128, sizeof *lex); // care
    uint  i   = 0;
    while (tb->begin != tb->lookahead) {
        char c   = tb_moveBegin(tb);
        lex[i++] = c;
    }

    return lex;
}

void tb_resetBegin(TwinBuffer *tb) {
    tb->begin         = tb->lookahead;
    tb->begin_buffnum = tb->lookahead_buffnum;
}
