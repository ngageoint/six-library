#pragma once

#if (defined(WIN32) || defined(_WIN32)) && defined(NITRO_PCH)

#undef HAVE_CLOCK_GETTIME
#undef HAVE_SYS_TIME_H

#else

#include "nrt/nrt_config.h"

#endif

#include "Version.h"
