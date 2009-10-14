/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include "nitf/SegmentWriter.hpp"

using namespace nitf;

SegmentWriter::SegmentWriter(nitf_SegmentWriter *x) throw (nitf::NITFException) :
    nitf::KnownWriteHandler(), mAdopt(false), mSegmentSource(NULL)
{
    setKnownHandler(x ? x : nitf_SegmentWriter_construct(&error));
}

SegmentWriter::~SegmentWriter()
{
    if (mAdopt && mSegmentSource)
    {
        mSegmentSource->decRef();
        delete mSegmentSource;
    }
}

void SegmentWriter::attachSource(nitf::SegmentSource* segmentSource, bool adopt)
        throw (nitf::NITFException)
{
    if (!nitf_SegmentWriter_attachSource(knownHandler,
            segmentSource->getNative(), &error))
        throw nitf::NITFException(&error);
    segmentSource->setManaged(true);
    segmentSource->incRef();
    mSegmentSource = segmentSource;
    mAdopt = adopt;
}
