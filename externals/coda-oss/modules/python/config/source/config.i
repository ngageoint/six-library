%module(package="coda") coda_config

#define static_assert(...) // SWIG <= 3.0.12 cannot handle static_assert
#define final // SWIG (as of 4.0.2) does not understand "final"

// Turn off exports with SWIG.  Use the compiler-specific macros (rather than a project-specifc name)
// so that this works in all projects 
#define __attribute__(...) // __GNUC__
#define __declspec(...) // _MSC_VER
#define CODA_OSS_library_export
#define CODA_OSS_library_import
#define CODA_OSS_API // ... well, except that it doesn't :-(
