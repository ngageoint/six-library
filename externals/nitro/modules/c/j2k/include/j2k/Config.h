#pragma once

#undef J2K_MODULE_EXPORTS

#if (defined(WIN32) || defined(_WIN32)) && defined(NITRO_PCH)
// NITRO_PCH is #defined when building in Visual Studio
#ifndef HAVE_OPENJPEG_H
#define HAVE_OPENJPEG_H 
#endif
#define J2K_MODULE_EXPORTS 

#else

#include "j2k/j2k_config.h"

#endif

