/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_LEGEND_H__
#define __SIX_LEGEND_H__

#include <vector>

#include <sys/Conf.h>
#include <types/RowCol.h>

namespace six
{
/*
 * This is a legend associated with a SIDD product
 * See section 2.4.3 of SIDD Volume 2
 *
 * Caller puts it in the Container but then I think we'll need to add this into
 * NITFImageInfo to give NITFWriteControl the chance to write it out
 */
struct Legend
{
public:
	// TODO: Or this could be PixelType and we throw if you give us an invalid
	//       one
	enum Type
	{
        RGB8LU = 0,
        RGB24I
	};

	Type mType;
	types::RowCol<size_t> mLocation;

	types::RowCol<size_t> mDims;
	std::vector<sys::ubyte> mImage;
};
}

#endif
