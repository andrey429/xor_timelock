#ifndef PACKER_H
#define PACKER_H
//includes
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include "bigd.h"
#include "bigdRand.h"
//constants
#ifndef PRIME_SIZE
#define PRIME_SIZE 128
#endif

#ifndef MODULO_SIZE
#define MODULO_SIZE 256
#endif

#ifndef MODULO_BYTES
#define MODULO_BYTES 32
#endif

#ifndef SEPARATOR
#define SEPARATOR "########"
#endif

#ifndef SEPARATOR_LEN
#define SEPARATOR_LEN 8
#endif
//structs
typedef struct TIMELOCK_STRUCT {
	//a, m - MODULO_SIZE in length
	BYTE *a, *n, *cm;
	DWORD t;
	DWORD cm_len;
} TIMELOCK;

#ifndef ERROR_OCCURED
#define ERROR_OCCURED (0x1)
#endif

#ifndef NO_PAYLOAD
#define NO_PAYLOAD (0x10)
#endif

#endif