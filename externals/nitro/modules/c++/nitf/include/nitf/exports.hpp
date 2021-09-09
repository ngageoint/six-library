#ifndef NITRO_nitf_exports_hpp_INCLUDED_
#define NITRO_nitf_exports_hpp_INCLUDED_
#pragma once

// Use Windows naming conventions (DLL, LIB) because this really only matters
// for _MSC_VER, see below.
#if !defined(NITRO_NITFCPP_LIB) && !defined(NITRO_NITFCPP_DLL)
    // Building a static library (not a DLL) is the default.
    #define NITRO_NITFCPP_LIB 1
#endif
#if !defined(NITRO_NITFCPP_DLL)
    #if defined(NITRO_NITFCPP_EXPORTS) && defined(NITRO_NITFCPP_LIB)
        #error "Can't export from a LIB'"
    #endif

    #if !defined(NITRO_NITFCPP_LIB)
        #define NITRO_NITFCPP_DLL 1
    #endif
#endif

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the NITRO_NITFCPP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// NITRO_NITFCPP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
// https://www.gnu.org/software/gnulib/manual/html_node/Exported-Symbols-of-Shared-Libraries.html
#if NITRO_NITFCPP_EXPORTS
    #if defined(__GNUC__) // && HAVE_VISIBILITY 
        #define NITRO_NITFCPP_API __attribute__((visibility("default")))
    #elif defined(_MSC_VER) // && (defined(_WINDLL) && !defined(_LIB))
        // Visual Studio projects define _WINDLL or _LIB, but not the compiler
        #define NITRO_NITFCPP_API __declspec(dllexport)
    #else
        // https://stackoverflow.com/a/2164853/8877
        #define NITRO_NITFCPP_API /* do nothing and hope for the best? */
        #pragma warning Unknown dynamic link export semantics.
    #endif
#else
    // Either building a static library (no NITRO_NITFCPP_EXPORTS) or
    // importing (not building) a shared library.
    #if defined(_MSC_VER)
        // We need to know whether we're consuming (importing) a DLL or static LIB
        // The default is a static LIB as that's what existing code/builds expect.
        #ifdef NITRO_NITFCPP_DLL
            // Actually, it seems that the linker is able to figure this out from the .LIB, so 
            // there doesn't seem to be a need for __declspec(dllimport).  Clients don't
            // need to #define NITRO_NITFCPP_DLL ... ?  Well, almost ... it looks
            // like __declspec(dllimport) is needed to get virtual "inline"s (e.g., 
            // destructors) correct.
            #define NITRO_NITFCPP_API __declspec(dllimport)
        #else
            #define NITRO_NITFCPP_API /* "importing" a static LIB */
        #endif
    #elif defined(__GNUC__)
        // For GCC, there's no difference in consuming ("importing") an .a or .so
        #define NITRO_NITFCPP_API /* no __declspec(dllimport) for GCC */
    #else
        // https://stackoverflow.com/a/2164853/8877
        #define NITRO_NITFCPP_API /* do nothing and hope for the best? */
        #pragma warning Unknown dynamic link import semantics.
    #endif
#endif

#if defined(_MSC_VER) && defined(NITRO_NITFCPP_DLL)
#pragma warning(disable: 4251) // '...' : class '...' needs to have dll-interface to be used by clients of struct '...'
#endif

#endif // NITRO_nitf_exports_hpp_INCLUDED_
