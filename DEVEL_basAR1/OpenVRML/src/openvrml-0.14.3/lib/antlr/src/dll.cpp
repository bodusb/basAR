/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/RIGHTS.html
 *
 * $Id: dll.cpp,v 1.1.1.1 2003/04/06 22:26:27 braden Exp $
 */

/*
 * DLL stub for MSVC 6. Based upon versions of Stephen Naughton and Michael
 * T. Richter
 */

#if _MSC_VER > 1000
# pragma once
#endif // _MSC_VER > 1000

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	return TRUE;
}
