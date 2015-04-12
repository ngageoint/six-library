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

#include "j2k/Container.h"


J2KAPI(nrt_Uint32) j2k_Container_getGridWidth(j2k_Container *container,
                                          nrt_Error *error)
{
    return container->iface->getGridWidth(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getGridHeight(j2k_Container *container,
                                           nrt_Error *error)
{
    return container->iface->getGridHeight(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getNumComponents(j2k_Container *container,
                                                  nrt_Error *error)
{
    return container->iface->getNumComponents(container->data, error);
}

J2KAPI(j2k_Component*) j2k_Container_getComponent(j2k_Container *container,
                                                  nrt_Uint32 idx,
                                                  nrt_Error *error)
{
    return container->iface->getComponent(container->data, idx, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getWidth(j2k_Container *container,
                                          nrt_Error *error)
{
    nrt_Uint32 nComponents, i, width, cWidth;
    nComponents = j2k_Container_getNumComponents(container, error);
    width = 0;
    for(i = 0; i < nComponents; ++i)
    {
        j2k_Component *c = j2k_Container_getComponent(container, i, error);
        cWidth = j2k_Component_getWidth(c, error);
        if (cWidth > width)
            width = cWidth;
    }
    return width;
}

J2KAPI(nrt_Uint32) j2k_Container_getHeight(j2k_Container *container,
                                           nrt_Error *error)
{
    nrt_Uint32 nComponents, i, height, cHeight;
    nComponents = j2k_Container_getNumComponents(container, error);
    height = 0;
    for(i = 0; i < nComponents; ++i)
    {
        j2k_Component *c = j2k_Container_getComponent(container, i, error);
        cHeight = j2k_Component_getHeight(c, error);
        if (cHeight > height)
            height = cHeight;
    }
    return height;
}

J2KAPI(nrt_Uint32) j2k_Container_getPrecision(j2k_Container *container,
                                              nrt_Error *error)
{
    nrt_Uint32 nComponents, i, precision, cPrecision;
    nComponents = j2k_Container_getNumComponents(container, error);
    precision = 0;
    for(i = 0; i < nComponents; ++i)
    {
        j2k_Component *c = j2k_Container_getComponent(container, i, error);
        cPrecision = j2k_Component_getPrecision(c, error);
        if (cPrecision > precision)
            precision = cPrecision;
    }
    return precision;
}

J2KAPI(nrt_Uint32) j2k_Container_getTilesX(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTilesX(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getTilesY(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTilesY(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getTileWidth(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTileWidth(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getTileHeight(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTileHeight(container->data, error);
}

J2KAPI(int) j2k_Container_getImageType(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getImageType(container->data, error);
}

J2KAPI(void)
j2k_Container_destruct(j2k_Container **container)
{
    if (*container)
    {
        if ((*container)->iface && (*container)->data)
            (*container)->iface->destruct((*container)->data);
        J2K_FREE(*container);
        *container = NULL;
    }
}

