#ifndef _datadef_
#define _datadef_

#ifndef BOOL

#define BOOL int
#define TRUE 1
#define FALSE 0

#endif

typedef float real;

#ifndef _countof
#define _countof(array) sizeof(array) / sizeof(*array)
#endif

#include <stdio.h>

#endif