#ifndef TWINBUFFER_H
#define TWINBUFFER_H

#include "utils.h"

#define BLOCKSZ 512

/**
 * Twin buffer data structure to read the source file
 * Also makes use of two pointers begin and lookahead
 * for efficient reading of lexemes
 */
typedef struct TwinBuffer {
    char  *buff[2];
    uint   used[2];
    char  *begin;
    char  *lookahead;
    int    begin_buffnum;
    int    lookahead_buffnum;
    FILE **fp;
} TwinBuffer;

/**
 * Load a block of the file into one
 * of the twin buffers
 */
void tb_loadNextBuff(TwinBuffer *tb);

/**
 * Consume a character from the twin buffer
 * Swap buffers if needed
 */
char tb_nextChar(TwinBuffer *tb, uint *linenum);

/**
 * Exude out a character from the twin buffer
 * Swap buffers if needed
 */
void tb_retract(TwinBuffer *tb, uint *linenum);

/**
 * Move the begin pointer (incase of a lexical error)
 */
char tb_moveBegin(TwinBuffer *tb);

/**
 * Get the string between the begin and lookahead pointers
 */
char *tb_getLexeme(TwinBuffer *tb);

/**
 * Match the begin pointer to the lookahead pointer
 */
void tb_resetBegin(TwinBuffer *tb);

#endif
