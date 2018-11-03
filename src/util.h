#ifndef _UTIL_H_
#define _UTIL_H_
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/* returns a buffer with file contents (dégoulasse) */
char *_file2mem (char *filename);

#endif
