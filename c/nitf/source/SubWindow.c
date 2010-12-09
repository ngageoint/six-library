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

#include "nitf/SubWindow.h"

NITFAPI(nitf_SubWindow *) nitf_SubWindow_construct(nitf_Error * error)
{
    nitf_SubWindow *subWindow =
        (nitf_SubWindow *) NITF_MALLOC(sizeof(nitf_SubWindow));
    if (!subWindow)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    subWindow->startRow = 0;
    subWindow->startCol = 0;
    subWindow->numRows = 0;
    subWindow->numCols = 0;

    subWindow->bandList = NULL;
    subWindow->numBands = 0;

    subWindow->downsampler = NULL;

    return subWindow;
}


NITFAPI(NITF_BOOL) nitf_SubWindow_setDownSampler(nitf_SubWindow *subWindow,
                                                 nitf_DownSampler *downsampler,
                                                 nitf_Error * error)
{
    assert(subWindow);
    subWindow->downsampler = downsampler;
    return NITF_SUCCESS;
}


NITFAPI(void) nitf_SubWindow_destruct(nitf_SubWindow ** subWindow)
{
    if (*subWindow)
    {
        NITF_FREE(*subWindow);
        *subWindow = NULL;
    }
}
