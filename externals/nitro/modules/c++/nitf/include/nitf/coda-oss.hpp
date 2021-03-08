/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

// Setup coda-oss for our use
#define CODA_OSS_AUGMENT_std_namespace 1 // add to std:: namespace
//#define CODA_OSS_Throwable_isa_std_exception 1 // except::Throwable derives from std::exception
#include <sys/Conf.h>
#include <except/Throwable.h>

#include <sys/Bit.h> // std::endian
#include <sys/CStdDef.h> // std::byte
#include <sys/Filesystem.h> // std::filesystem

//--------------------------------------------------------------
#include <import/except.h>
#include <import/mt.h>
#include <import/sys.h>
#include <import/str.h>
#include <import/types.h>
#include <import/io.h>
#include <import/mem.h>
#include <gsl/gsl.h>

