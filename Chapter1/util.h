//
// Created by Boris Chen on 19/7/2020.
//

#ifndef COMPILERS_UTIL_H
#define COMPILERS_UTIL_H

#include <assert.h>

typedef char *string;
string String(char *);

typedef char bool;
#define TRUE 1
#define FALSE 0

void *check_malloc(int);
#endif //COMPILERS_UTIL_H
