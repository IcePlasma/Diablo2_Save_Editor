#include "D2SaveEditor.h"
#include <iostream>
#include <string>
using namespace std;

//Skills come after a header of "if"
//Original checksum function.
/*
void __fastcall DUMPER_FixChecksum(BYTE* pFile, DWORD dwSize)
{
	if (pFile == 0 || dwSize < 0)
		return;

	DWORD* pSignature = (DWORD*)(pFile + 0xC);
	*pSignature = 0;

	int nSignature = 0;
	for (DWORD i = 0; i < dwSize; i++) {
		int byte = pFile[i];
		if (nSignature < 0)
			byte++;
		nSignature = byte + nSignature * 2;
	}
	*pSignature = nSignature;
}
*/

int main(int argc, char* argv)
{
	D2SaveEditor* mySaveEditor = new D2SaveEditor();
	string pathString;
	if (argc > 1)
	{
		pathString = argv[1];
		mySaveEditor->RunEditor(pathString);
	}
	else
	{
		mySaveEditor->RunEditor();
	}
	delete mySaveEditor;
	return 0;
}
