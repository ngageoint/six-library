/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * config-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef CONFIG_COMPILER_EXTENSIONS
#define CONFIG_COMPILER_EXTENSIONS

#include <config/coda_oss_config.h>

#ifdef HAVE_ATTRIBUTE_NOINLINE
#define ATTRIBUTE_NOINLINE __attribute__((noinline))
#else
#define ATTRIBUTE_NOINLINE
#endif

#ifdef HAVE_ATTRIBUTE_ALIGNED
#define ATTRIBUTE_ALIGNED(x) __attribute__((aligned (x)))
#else
#define ATTRIBUTE_ALIGNED(X)
#endif

#endif
