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

#ifndef __NITF_IMAGE_READER_H__
#define __NITF_IMAGE_READER_H__

#include "nitf/ImageIO.h"
#include "nitf/System.h"

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#endif
typedef struct _nitf_ImageReader
{
    nitf_IOInterface* input;
    nitf_ImageIO *imageDeblocker;
    int directBlockRead;
}
nitf_ImageReader;
#if _MSC_VER
#pragma warning(pop)
#endif

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_BlockingInfo *)
nitf_ImageReader_getBlockingInfo(nitf_ImageReader * imageReader,
                                 nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(NITF_BOOL) nitf_ImageReader_read(nitf_ImageReader * imageReader,
        nitf_SubWindow * subWindow,
        uint8_t ** user,
        int *padded, nitf_Error * error);

/**
   Read a block directly from file
 */
NITFAPI(uint8_t*) nitf_ImageReader_readBlock(nitf_ImageReader * imageReader,
                                                uint32_t blockNumber,
                                                uint64_t* blockSize,
                                                nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(void) nitf_ImageReader_destruct(nitf_ImageReader ** imageReader);

/*!
  \brief nitf_ImageReader_setReadCaching - Enable cached reads

  nitf_ImageReader_setReadCaching enables cached reads. Enabling cached reads
  causes the system to read full blocks of data and then satisfy individual
  read requests from these buffered blocks. A cache of block buffers is
  maintained. This is usually more efficent in terms of reading but requires
  more memory.

  \return None
*/

NITFAPI(void) nitf_ImageReader_setReadCaching
(
    nitf_ImageReader * iReader  /*!< Object to modify */
);

NITF_CXX_ENDGUARD

#endif
