/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
<<<<<<<< HEAD:externals/nitro/modules/c/nitf/shared/PRADAA.c
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
========
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2017, MDA Information Systems LLC
>>>>>>>> 2c7b84b5157cb36063ccdf447b621b14e1b2cb6c:externals/nitro/modules/c++/nitf/source/J2KStream.cpp
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

<<<<<<<< HEAD:externals/nitro/modules/c/nitf/shared/PRADAA.c
#include <import/nitf.h>

NITF_CXX_GUARD

// MIL-PRF-89034, Table 9 (page 48).
static nitf_TREDescription description[] = {
    {NITF_BCS_A, 8, "DPPDB Absolute CE 90%", "PACE" },
    {NITF_BCS_A, 8, "DPPDB Absolute LE 90%", "PALE" },
    {NITF_BCS_A, 8, "DPPDB Support Data CE 90%", "PSDCE" },
    {NITF_BCS_A, 8, "DPPDB Support Data LE 90%", "PSDLE" },
    {NITF_BCS_A, 8, "DPPDB Mensuration 1 pixel CE 90%", "PMCE" },
    {NITF_BCS_A, 8, "DPPDB Mensuration 1 pixel LE 90%", "PMLE" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PRADAA, description)

NITF_CXX_ENDGUARD
========
#include "nitf/J2KStream.hpp"

#include <sstream>
#include <stdexcept>

#include "nitf/NITFException.hpp"

j2k::Stream::Stream(j2k::StreamType streamType, size_t chunkSize)
{
    const auto isInputStream = streamType == j2k::StreamType::INPUT;

    mStream = j2k_stream_create(chunkSize, isInputStream);
    if (!mStream)
    {
        std::ostringstream os;
        os << "Failed creating an openjpeg stream with a chunk size of " << chunkSize << " bytes.";
        throw except::Exception(Ctxt(os.str()));
    }
}

j2k::Stream::~Stream()
{
    j2k_stream_destroy(mStream);
}
>>>>>>>> 2c7b84b5157cb36063ccdf447b621b14e1b2cb6c:externals/nitro/modules/c++/nitf/source/J2KStream.cpp
