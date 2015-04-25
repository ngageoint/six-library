/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
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

/*
  \file DirectBlockSource - Band source implementation for data read directly

  The DirectBlockSource object is derived from the DataSource object and is used 
  in cases when the user wants to directly read blocks from file without
  requiring any re-formatting or re-organization.  One such case would be a file 
  copy.  Another would be some sort of block-level data merge where data from 
  all sources is formatted the same.

  This allows higher performance reading and writing in such cases where the 
  user knows the data is formatted a certain way.
*/

#ifndef __NITF_DIRECT_BLOCK_SOURCE_H__
#define __NITF_DIRECT_BLOCK_SOURCE_H__

#include "nitf/BandSource.h"

NITF_CXX_GUARD

/*!
  \brief NITF_DIRECT_BLOCK_SOURCE_NEXT_BLOCK - Next row function prototype

  \param algorithm   - The algorithm object
  \param buf         - The buffer to receive the data
  \param block       - The block read from file
  \param blockNumber - The block number
  \param blockSize   - The block size
  \param error       - Error object for error returns

  \return Returns true on success. On failure the error object is set
*/
typedef NITF_BOOL(*NITF_DIRECT_BLOCK_SOURCE_NEXT_BLOCK) (void* algorithm,
                                                         void* buf,
                                                         const void* block,
                                                         nitf_Uint32 blockNumber,
                                                         nitf_Uint64 blockSize,
                                                         nitf_Error * error);

/*
  \brief nitf_DirectBlockSource_construct - Constructor for the DirectBlockSource 
  object

  \return The new object or NULL on error. On error, the error object is
  set
*/
NITFAPI(nitf_BandSource *) nitf_DirectBlockSource_construct(void * algorithm,
                                                            NITF_DIRECT_BLOCK_SOURCE_NEXT_BLOCK
                                                            nextBlock,
                                                            nitf_ImageReader* imageReader,
                                                            nitf_Uint32 numBands,
                                                            nitf_Error * error);

NITF_CXX_ENDGUARD
#endif
