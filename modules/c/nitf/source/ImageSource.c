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

#include "nitf/ImageSource.h"

NITFAPI(nitf_ImageSource *) nitf_ImageSource_construct(nitf_Error * error)
{
    nitf_ImageSource *imageSource =
        (nitf_ImageSource *) NITF_MALLOC(sizeof(nitf_ImageSource));
    if (!imageSource)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    imageSource->bandSources = nitf_List_construct(error);
    if (!imageSource->bandSources)
    {
        NITF_FREE(imageSource);
        return NULL;
    }
    imageSource->size = 0;
    return imageSource;
}


NITFAPI(void) nitf_ImageSource_destruct(nitf_ImageSource ** imageSource)
{
    if (*imageSource)
    {
        nitf_List *l = (*imageSource)->bandSources;
        while (!nitf_List_isEmpty(l))
        {
            nitf_BandSource *bandSource =
                (nitf_BandSource *) nitf_List_popFront(l);
            nitf_BandSource_destruct(&bandSource);
        }
        nitf_List_destruct(&l);
        NITF_FREE(*imageSource);
        *imageSource = NULL;
    }
}


NITFAPI(NITF_BOOL)
nitf_ImageSource_addBand(nitf_ImageSource * imageSource,
                         nitf_BandSource * bandSource, nitf_Error * error)
{
    if (!nitf_List_pushBack(imageSource->bandSources, bandSource, error))
        return NITF_FAILURE;
    ++imageSource->size;
    return NITF_SUCCESS;
}


NITFAPI(nitf_BandSource *)
nitf_ImageSource_getBand(nitf_ImageSource * imageSource,
                         int n, nitf_Error * error)
{
    int i = 1;
    nitf_ListIterator it;
    if (n < 0 || n >= imageSource->size)
    {
        nitf_Error_init(error,
                        "Error: band out of range",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }
    it = nitf_List_begin(imageSource->bandSources);

    for (; i <= n; i++)
        nitf_ListIterator_increment(&it);
    return (nitf_BandSource *) nitf_ListIterator_get(&it);
}
