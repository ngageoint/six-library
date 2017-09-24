/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_SIDD_SIDD_BYTE_PROVIDER_H__
#define __SIX_SIDD_SIDD_BYTE_PROVIDER_H__


// - Methods that provide image data must provide rows of a multiple of the block size!
//   This means two things though...
//   1. The caller must know where the segmentation boundaries are in order to
//      provide pad pixels at the bottom of the right spot.  They may have pixel
//      data that spans image segments, and they need to put those pad pixels
//      in there themselves.
//   2. The logic in ByteProvider needs updating to account for this... the way
//      it counts rows isn't quite right and will need to know about these
//      extra pad rows.  The mNumBytesPerRow concept itself probably doesn't
//      apply quite right anymore.  This affects getNumBytes() too.
// - Include a test case where the blocking is set to a value > the image dimensions
//   According to the NITFWriteControl logic, the block will actually match the image dims

#endif /* SIDDBYTEPROVIDER_H_ */
