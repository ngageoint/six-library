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

J2KAPI(nrt_Uint32) j2k_Container_getWidth(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getWidth(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getHeight(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getHeight(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getNumComponents(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getNumComponents(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getComponentBytes(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getComponentBytes(container->data, error);
}

J2KAPI(nrt_Uint32) j2k_Container_getComponentBits(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getComponentBits(container->data, error);
}

J2KAPI(int) j2k_Container_getImageType(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getImageType(container->data, error);
}

J2KAPI(NRT_BOOL) j2k_Container_isSigned(j2k_Container *container, nrt_Error *error)
{
    return container->iface->isSigned(container->data, error);
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

