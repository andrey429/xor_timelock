#ifndef UNPACKER_H
#define UNPACKER_H
//includes
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include "bigd.h"
//constants
#ifndef MODULO_BYTES
#define MODULO_BYTES 32
#endif

#ifndef SEPARATOR
#define SEPARATOR "########"
#endif

#ifndef SEPARATOR_LEN
#define SEPARATOR_LEN 8
#endif

#ifndef CODE_ERROR
#define CODE_ERROR -1
#endif

#ifndef ERROR_OCCURED
#define ERROR_OCCURED (0x1)
#endif

#ifndef NO_PAYLOAD
#define NO_PAYLOAD (0x10)
#endif
#ifndef NAME
#define NAME "pl.txt"
#endif

#endif