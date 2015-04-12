/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_IMAGE_SOURCE_H__
#define __NITF_IMAGE_SOURCE_H__

#include "nitf/System.h"
#include "nitf/BandSource.h"

NITF_CXX_GUARD

/*!
 *  \struct nitf_ImageSource
 *  \brief The base for harnessing a source.
 *
 *  The image source holds a BandSource for every band.
 *  In the single band solution, this is particularly straightforward
 *  and performance oriented.  However, it also satisfies our requirement
 *  to keep the bands separate, without require them to live in core.
 *
 *  This object will be added to the ImageSegment using the
 *  addImageSource() method
 */
typedef struct _nitf_ImageSource
{
    nitf_List *bandSources;
    int size;
}
nitf_ImageSource;

/*  ImageSources cannot be cloned!!  */

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_ImageSource *) nitf_ImageSource_construct(nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(void) nitf_ImageSource_destruct(nitf_ImageSource **);

/*!
 *  TODO: Add documentation
 */
NITFAPI(NITF_BOOL)
nitf_ImageSource_addBand(nitf_ImageSource * imageSource,
                         nitf_BandSource * bandSource, nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(nitf_BandSource *)
nitf_ImageSource_getBand(nitf_ImageSource * imageSource,
                         int n, nitf_Error * error);


NITF_CXX_ENDGUARD

#endif
