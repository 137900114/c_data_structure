#ifndef _helper_
#define _helper_

#ifndef BOOL

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#endif

typedef float real;

#ifndef _countof
#define _countof(array) sizeof(array) / sizeof(*array)
#endif

#include <stdio.h>

#endif