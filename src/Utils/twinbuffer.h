#ifndef TWINBUFFER_H
#define TWINBUFFER_H

#include "utils.h"

#define BLOCKSZ 512

typedef struct TwinBuffer {
    char  *buff[2];
    uint   used[2];
    char  *begin;
    char  *lookahead;
    int    begin_buffnum;
    int    lookahead_buffnum;
    FILE **fp;
} TwinBuffer;

void  tb_loadNextBuff(TwinBuffer *tb);
char  tb_nextChar(TwinBuffer *tb, uint *linenum);
void  tb_retract(TwinBuffer *tb, uint *linenum);
char  tb_moveBegin(TwinBuffer *tb);
char *tb_getLexeme(TwinBuffer *tb);
void  tb_resetBegin(TwinBuffer *tb);
void  _prinbuf(TwinBuffer *tb);

#endif
