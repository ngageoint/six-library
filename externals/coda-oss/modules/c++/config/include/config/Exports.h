#ifndef CODA_OSS_config_Exports_h_INCLUDED_
#define CODA_OSS_config_Exports_h_INCLUDED_
#pragma once

#include "config/compiler_extensions.h"

// Need to specify how this code will be consumed, either CODA_OSS_LIB (static library)
// or CODA_OSS_DLL (aka "shared" library).  For DLLs, it needs to be set for BOTH
// "exporting" (building this code) and "importing" (consuming).
//
// Use Windows naming conventions (DLL, LIB) because this really only matters for _MSC_VER, see below.
#if !defined(CODA_OSS_LIB) && !defined(CODA_OSS_DLL)
    #define CODA_OSS_LIB 1
    //#define CODA_OSS_DLL 1
#endif
#if defined(CODA_OSS_LIB) && defined(CODA_OSS_DLL)
    #error "Both CODA_OSS_LIB and CODA_OSS_DLL are #define'd'"
#endif
#if defined(CODA_OSS_EXPORTS) && defined(CODA_OSS_LIB)
    #error "Can't export from a LIB'"
#endif

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the CODA_OSS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// CODA_OSS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
// https://www.gnu.org/software/gnulib/manual/html_node/Exported-Symbols-of-Shared-Libraries.html
#ifdef CODA_OSS_EXPORTS
    #define CODA_OSS_API CODA_OSS_library_export
#else
    // Either building a static library (no CODA_OSS_EXPORTS) or
    // importing (not building) a shared library.

    // We need to know whether we're consuming (importing) a DLL or static LIB
    // The default is a static LIB as that's what existing code/builds expect.
    #ifdef CODA_OSS_DLL
        // Actually, it seems that the linker is able to figure this out from the .LIB, so 
        // there doesn't seem to be a need for __declspec(dllimport).  Clients don't
        // need to #define CODA_OSS_DLL ... ?  Well, almost ... it looks
        // like __declspec(dllimport) is needed to get virtual "inline"s (e.g., 
        // destructors) correct.
        #define CODA_OSS_API CODA_OSS_library_import
    #else
        #define CODA_OSS_API /* "importing" a static LIB */
    #endif
#endif

#if defined(_MSC_VER)
#pragma warning(disable: 4251) // '...' : class '...' needs to have dll-interface to be used by clients of struct '...'
#endif

#endif // CODA_OSS_config_Exports_h_INCLUDED_
