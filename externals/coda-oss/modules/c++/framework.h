#pragma once

#pragma warning(push)
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 4514) //	'...': unreferenced inline function has been removed
#pragma warning(disable: 4365) //	'...': conversion from '...' to '...', signed/unsigned mismatch
#pragma warning(disable: 5204) //	'...': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly	
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX // no min()/max() macros
#include <windows.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS // '...': Use getaddrinfo() or GetAddrInfoW() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings
#include <Winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32")

#include <comdef.h>

#pragma warning(pop)

