#ifndef __STDAFX_H_INCLUDED
#define __STDAFX_H_INCLUDED

#ifndef _CONSOLE
#	define _CONSOLE
#endif

#ifndef _DEBUG
#	define _DEBUG
#endif

#define WITH_ASSERTS
#define CONSOLE_PREFIX "[" __FILE__ ":" S__LINE__ "] : "


#include <platform.h>
#include <types.h>
#include <asserts.h>

#include <iostream>
#include <iomanip>
#include <string.h>

using namespace std;

#define SMARTY_VERSION	0x00010000

#ifdef WINDOWS
#   pragma warning(disable : 4101)
#endif

#endif // __STDAFX_H_INCLUDED
