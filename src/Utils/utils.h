#ifndef UTILS_H
#define UTILS_H

// required standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// get the size of an array (whose size is fixed at compile time)
#define NUM_ELEM(x) (sizeof(x) / sizeof((x)[0]))

#endif
