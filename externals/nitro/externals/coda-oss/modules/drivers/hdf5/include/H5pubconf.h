/* src/H5config.h.  Generated from H5config.h.in by configure.  */
/* src/H5config.h.in.  Generated from configure.ac by autoheader.  */

#pragma once
#ifndef H5pubconf_h_
#define H5pubconf_h_

#if defined(_WIN32)
#include "H5pubconf_WIN32.h"

#else
#include <features.h>
#if defined(_POSIX_C_SOURCE)
#include "H5pubconf_POSIX.h"

#else
#error "Unknown platform."
#endif

#endif

#endif // H5pubconf_h_
