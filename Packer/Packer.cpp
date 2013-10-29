#include "Packer.h"

VOID ShowResult(DWORD dwCode)
{
	if	(dwCode == ERROR_OCCURED)
	{
		_tprintf(TEXT("Error occured\n"));
	}
	else if	(dwCode == NO_PAYLOAD)
	{
		_tprintf(TEXT("No payload\n"));
	}
	else
	{
		_tprintf(TEXT("Success"));
	}
}
VOID GeneratePrime(BIGD p)
{
	bdRandomBits(p, PRIME_SIZE);
	while	(!bdIsPrime	(p, 10))
	{
		bdIncrement(p);
	}
}
VOID ProduceTimelock(DWORD log_t, PBYTE data, DWORD data_len, TIMELOCK& timelock)
{
	//declaration
	BIGD a, n, p, q, phi, e;
	DWORD t;	
	//initialise
	a = bdNew();
	n = bdNew();
	p = bdNew();
	q = bdNew();
	phi = bdNew();
	e = bdNew();
	//generate modulo, t, a
	t = 1 << log_t;
	GeneratePrime(p);
	GeneratePrime(q);
	bdMultiply(n, p, q);
	bdDecrement(p);
	bdDecrement(q);
	bdMultiply(phi, p, q);	
	//temp
	bdSetShort(a, 2);
	bdSetShort(q, t);
	//compute one-way function
	bdModExp(e, a, q, phi); //e := 2^t (mod phi(n))
	bdRandomBits(a, MODULO_SIZE);
	//----
	PBYTE a_str = (PBYTE) malloc (MODULO_BYTES * sizeof(BYTE) );//original a to store
	bdConvToOctets(a, a_str, MODULO_BYTES);
	//----
	bdModExp(a, a, e, n); //a := a^e (mod n)
	//convert a,n to raw bytes - a_str, n_str
	PBYTE n_str = (PBYTE) malloc (MODULO_BYTES * sizeof(BYTE) );
	bdConvToOctets(n, n_str, MODULO_BYTES);
	PBYTE key = (PBYTE) malloc (MODULO_BYTES * sizeof(BYTE));
	bdConvToOctets(a, key, MODULO_BYTES);
	//xor cipher with block length 32
	int block_len = MODULO_BYTES;
	int blocks = data_len / block_len;
	for	(int i = 0; i < blocks; i++)
	{
		for	(int j = 0; j < block_len; j++)
		{
			data[i*block_len + j] ^= key[j];
		}
	}
	//fill timelock struct
	timelock.a = a_str;
	timelock.n = n_str;
	timelock.cm = data;
	timelock.cm_len = data_len;
	timelock.t = t;
	//destroy everything else
	bdFree(&a);
	bdFree(&p);
	bdFree(&q);
	bdFree(&n);
	bdFree(&phi);
	bdFree(&e);
	free(key);

}

void DestroyTimelock(TIMELOCK& timelock)
{
	memset(timelock.a, 0, MODULO_BYTES);
	memset(timelock.cm, 0, MODULO_BYTES);
	memset(timelock.n, 0, MODULO_BYTES);
	timelock.t = 0;
	free(timelock.a);
	free(timelock.cm);
	free(timelock.n);
}


/*
Writes structure in format:
SEPARATOR__SEPARATOR__t__a__n__cm
t: 4 bytes
a, n: 32 bytes
cm: cm_len bytes
*/
VOID InjectTimelock(const TIMELOCK timelock, const PCHAR path_to)
{
	HANDLE hFile = CreateFileA(path_to, FILE_APPEND_DATA, 0, NULL, OPEN_EXISTING, 0, NULL);
	if	(hFile == INVALID_HANDLE_VALUE)
	{
		ShowResult(ERROR_OCCURED);
		CloseHandle(hFile);
		return;
	}
	BOOL bRes;
	DWORD dwCounter;
	bRes = WriteFile(hFile, SEPARATOR, SEPARATOR_LEN, &dwCounter, NULL);
	bRes &= WriteFile(hFile, SEPARATOR, SEPARATOR_LEN, &dwCounter, NULL);
	bRes &= WriteFile(hFile, &timelock.t, sizeof(DWORD), &dwCounter, NULL);
	bRes &= WriteFile(hFile, timelock.a, MODULO_BYTES, &dwCounter, NULL);
	bRes &= WriteFile(hFile, timelock.n, MODULO_BYTES, &dwCounter, NULL);
	bRes &= WriteFile(hFile, timelock.cm, timelock.cm_len, &dwCounter, NULL);
	if	(!bRes)
	{
		ShowResult(ERROR_OCCURED);
		CloseHandle(hFile);
		return;
	}
	else
	{
		CloseHandle(hFile);
		ShowResult(0);
		return;
	}
}
VOID PrintUsage(){
	_tprintf(TEXT("Usage:\nPacker.exe [payload_path] [target_unpacker_path] [log_t]\n"));
	getchar();
}
VOID ReadToBuffer(const PCHAR path, PBYTE& buffer, DWORD& buf_len)
{
	HANDLE hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if	(hFile == INVALID_HANDLE_VALUE)
	{
		ShowResult(ERROR_OCCURED);
		CloseHandle(hFile);
		return;
	}
	DWORD dwCounter;
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	buffer = (PBYTE) malloc (dwFileSize * sizeof(BYTE));
	if	(!ReadFile(hFile, buffer, dwFileSize, &dwCounter, NULL))
	{
		ShowResult(ERROR_OCCURED);
	}
	buf_len = dwFileSize;
	CloseHandle(hFile);
	return;
}
int _tmain(int argc, _TCHAR* argv[])
{
	if(argc!=4)
	{
		PrintUsage();
		return 0;
	}
	//declare
	TIMELOCK timelock;
	PBYTE data;
	DWORD data_len;
	const PCHAR payload_path = argv[1];
	const PCHAR target_path = argv[2];
	DWORD log_t = (DWORD) atoi (argv[3]);	
	ReadToBuffer(payload_path, data, data_len);
	ProduceTimelock(log_t, data, data_len, timelock);
	InjectTimelock(timelock, target_path);
	DestroyTimelock(timelock);
	return 0;
}