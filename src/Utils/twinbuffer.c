
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "twinbuffer.h"

TwinBuffer *tb_init(FILE *fp) {

    TwinBuffer *tb = malloc(sizeof *tb);

    *tb = (TwinBuffer){
        .used[0]           = 0,
        .used[1]           = 0,
        .begin_buffnum     = 0,
        .lookahead_buffnum = 0,
        .fp                = fp,
    };

    tb->begin     = tb->buff[tb->begin_buffnum];
    tb->lookahead = tb->buff[tb->lookahead_buffnum];

    return tb;
}

void tb_loadNextBuff(TwinBuffer *tb) {

    // read upto BLOCKSZ bytes from file
    uint bytes_read = fread(tb->buff[tb->lookahead_buffnum], 1, BLOCKSZ, tb->fp);
    if (ferror(tb->fp)) {
        perror("Error when initializing buffer");
        exit(EXIT_FAILURE);
    }

    // if the file pointer has reached EOF, it is made known to the tb
    if (feof(tb->fp)) {
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
        tb->lookahead_buffnum = !tb->lookahead_buffnum;  // switch buffers
        tb_loadNextBuff(tb);                             // read from file
        tb->lookahead = tb->buff[tb->lookahead_buffnum]; // move to the beginning of the new buffer
    }

    return c;
}

void tb_retract(TwinBuffer *tb, uint *linenum) {

    // underflow - switch buffers and move file pointer back
    if (tb->lookahead == tb->buff[tb->lookahead_buffnum]) {
        long seek_back = (long)(tb->used[tb->lookahead_buffnum]) * -1; // how much to move file pointer back
        fseek(tb->fp, seek_back, SEEK_CUR);                            // move the file pointer
        tb->lookahead_buffnum = !tb->lookahead_buffnum;                // switch buffers
        tb->lookahead =
            tb->buff[tb->lookahead_buffnum] + tb->used[tb->lookahead_buffnum] - 1; // update lookahead pointer
    }

    else {
        tb->lookahead--;
    }

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
    char tmp_buf[TMP_BUFLEN];
    uint i = 0;
    while (tb->begin != tb->lookahead) {
        char c       = tb_moveBegin(tb);
        tmp_buf[i++] = c;
    }

    tmp_buf[i++] = '\0';
    char *lex    = calloc(i, sizeof *lex);
    memcpy(lex, tmp_buf, i);

    return lex;
}

void tb_resetBegin(TwinBuffer *tb) {
    tb->begin         = tb->lookahead;
    tb->begin_buffnum = tb->lookahead_buffnum;
}
