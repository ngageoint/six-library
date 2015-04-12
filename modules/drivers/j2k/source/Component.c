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

#include "j2k/Component.h"

J2KAPI(nrt_Uint32) j2k_Component_getWidth(j2k_Component *component,
                                          nrt_Error *error)
{
    return component->iface->getWidth(component->data, error);
}

J2KAPI(nrt_Uint32) j2k_Component_getHeight(j2k_Component *component,
                                           nrt_Error *error)
{
    return component->iface->getHeight(component->data, error);
}

J2KAPI(nrt_Uint32) j2k_Component_getPrecision(j2k_Component *component,
                                              nrt_Error *error)
{
    return component->iface->getPrecision(component->data, error);
}

J2KAPI(NRT_BOOL) j2k_Component_isSigned(j2k_Component *component,
                                        nrt_Error *error)
{
    return component->iface->isSigned(component->data, error);
}

J2KAPI(nrt_Int32) j2k_Component_getOffsetX(j2k_Component *component,
                                            nrt_Error *error)
{
    return component->iface->getOffsetX(component->data, error);
}

J2KAPI(nrt_Int32) j2k_Component_getOffsetY(j2k_Component *component,
                                            nrt_Error *error)
{
    return component->iface->getOffsetY(component->data, error);
}

J2KAPI(nrt_Uint32) j2k_Component_getSeparationX(j2k_Component *component,
                                                nrt_Error *error)
{
    return component->iface->getSeparationX(component->data, error);
}

J2KAPI(nrt_Uint32) j2k_Component_getSeparationY(j2k_Component *component,
                                                nrt_Error *error)
{
    return component->iface->getSeparationY(component->data, error);
}

J2KAPI(void)
j2k_Component_destruct(j2k_Component **component)
{
    if (*component)
    {
        if ((*component)->iface && (*component)->data)
            (*component)->iface->destruct((*component)->data);
        J2K_FREE(*component);
        *component = NULL;
    }
}

