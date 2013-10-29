#include "Unpacker.h"

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
DWORD LocateSeparator(const PBYTE pBuf, DWORD dwBufLen)
{
	DWORD pos;
	for	(pos = 0; pos < (dwBufLen - SEPARATOR_LEN*2); pos++)
	{
		if	(
			!memcmp(	(PBYTE)((DWORD)pBuf + pos), SEPARATOR, SEPARATOR_LEN	) &&
			!memcmp(	(PBYTE)((DWORD)pBuf + pos + SEPARATOR_LEN), SEPARATOR, SEPARATOR_LEN	)
			)
		{
			break;
		}
	}
	return pos < (dwBufLen - SEPARATOR_LEN) ? pos : CODE_ERROR;
}
VOID Decrypt(PBYTE a_str, PBYTE n_str, DWORD t, PBYTE& cm_str, DWORD cm_len)
{
	BIGD a, n, two;
	a = bdNew();
	n = bdNew();
	two = bdNew();
	bdConvFromOctets(a, a_str, 32);
	bdConvFromOctets(n, n_str, 32);
	bdSetShort(two, 2);
	while	(t > 0)
	{
		bdModExp(a, a, two, n);
		t--;
	}	
	PBYTE key = (PBYTE) malloc (32 * sizeof(BYTE));
	bdConvToOctets(a, key, 32);
	DWORD dwBlocks = cm_len / MODULO_BYTES;
	for	(DWORD dwI = 0; dwI < dwBlocks; dwI++)
	{
		for	(int j = 0; j < MODULO_BYTES; j++)
		{
			cm_str[dwI*MODULO_BYTES + j] ^= key[j];
		}
	}
	bdFree(&a);
	bdFree(&n);
	bdFree(&two);
	free(key);
}
VOID ReleaseCode(const PCHAR path)
{
	HANDLE hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	while(hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		return;
	}
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	PBYTE buffer = (PBYTE) malloc (dwFileSize * sizeof (BYTE));
	DWORD dwCounter;
	if	(!ReadFile(hFile, buffer, dwFileSize, &dwCounter, NULL))
	{
		CloseHandle(hFile);
		free(buffer);
		return;
	}
	DWORD i = LocateSeparator(buffer, dwFileSize);
	if	(i >= (dwFileSize - SEPARATOR_LEN*2))
	{
		CloseHandle(hFile);
		free(buffer);
		ShowResult(NO_PAYLOAD);
		return;
	}
	//parse
	PBYTE pTemp = (PBYTE) ((DWORD) buffer + i + 2*SEPARATOR_LEN);
	DWORD t;
	PBYTE a = (PBYTE) malloc (32 * sizeof(BYTE));
	PBYTE n = (PBYTE) malloc (32 * sizeof(BYTE));
	DWORD cm_len = dwFileSize - i - 2*SEPARATOR_LEN - 64*sizeof(BYTE) - 4*sizeof(BYTE);
	PBYTE cm = (PBYTE) malloc(cm_len * sizeof(BYTE));
	memcpy(&t, pTemp, 4*sizeof(BYTE));
	pTemp += 4*sizeof(BYTE);
	memcpy(a, pTemp, 32*sizeof(BYTE));
	pTemp += 32*sizeof(CHAR);
	memcpy(n, pTemp, 32*sizeof(BYTE));
	pTemp += 32*sizeof(BYTE);
	memcpy(cm, pTemp, cm_len);
	//unlock
	Decrypt(a, n, t, cm, cm_len);
	///run
	HANDLE hPL = CreateFileA(NAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	WriteFile(hPL, cm, cm_len, &t, NULL);
	CloseHandle(hPL);
	{
		CloseHandle(hFile);
		free(buffer);
		free(a);
		free(n);
		free(cm);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	ReleaseCode(argv[0]);
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	CreateProcessA(NAME, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &si, &pi);
	return 0;
}

