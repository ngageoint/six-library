/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CPHD_BYTE_SWAP_H__
#define __CPHD_BYTE_SWAP_H__

#include <stddef.h>

#include <types/Complex.h>
#include <types/RowCol.h>

#include <scene/sys_Conf.h>

#include "cphd/Types.h"

namespace cphd
{
/*
 *  \func byteSwap
 *  \brief Threaded byte-swapping
 *
 *  \param buffer Buffer to swap (contents will be overridden)
 *  \param elemSize Size of each element in 'buffer'
 *  \param numElements Number of elements in 'buffer'
 *  \param numThreads Number of threads to use for byte-swapping
 */
void byteSwap(void* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads);

/*
 *  \func byteSwapAndPromote
 *  \brief Threaded byte-swapping and promote input to complex<floats>
 *
 *  Valid input types:
 *     int8_t
 *     int16_t
 *     sys::float
 *
 *  \param input Input to swap and promote
 *  \param elementSize Size of each element in 'input'
 *  \param dims Number of rows and cols of elements in 'input'
 *  \param numThreads Number of threads to use for byte-swapping
 *  \param output Pointer to output array of complex<float>
 *
 *  \throws If elementSize is not one of (2,4 or 8)
 */
void byteSwapAndPromote(const void* input,
                        size_t elementSize,
                        const types::RowCol<size_t>& dims,
                        size_t numThreads,
                        cphd::zfloat* output);

/*
 *  \func byteSwapAndScale
 *  \brief Threaded byte-swapping and promote input to complex<floats>
 *
 *  Valid input types:
 *     int8_t
 *     int16_t
 *     sys::float
 *
 *  \param input Input to swap, promote and scale
 *  \param elementSize Size of each element in 'input'
 *  \param dims Number of rows and cols of elements in 'input'
 *  \param scaleFactors pointer to num rows size array of doubles
 *         to scale the input
 *  \param numThreads Number of threads to use for byte-swapping
 *  \param output Pointer to output array of scaled complex<float>
 *
 *  \throws If elementSize is not one of (2,4 or 8)
 */
void byteSwapAndScale(const void* input,
                      size_t elementSize,
                      const types::RowCol<size_t>& dims,
                      const double* scaleFactors,
                      size_t numThreads,
                      cphd::zfloat* output);
}

#endif
