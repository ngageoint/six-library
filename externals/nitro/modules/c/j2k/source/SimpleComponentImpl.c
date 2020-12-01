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

#include "j2k/Component.h"


typedef struct _ComponentImpl
{
    uint32_t width;
    uint32_t height;
    uint32_t precision;
    NRT_BOOL isSigned;
    int32_t x0;
    int32_t y0;
    uint32_t xSeparation;
    uint32_t ySeparation;

} ComponentImpl;

J2KPRIV( uint32_t) Component_getWidth(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t) Component_getHeight(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t) Component_getPrecision(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( J2K_BOOL)   Component_isSigned(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( int32_t)  Component_getOffsetX(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( int32_t)  Component_getOffsetY(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t) Component_getSeparationX(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t) Component_getSeparationY(J2K_USER_DATA *, nrt_Error *);
J2KPRIV(void)        Component_destruct(J2K_USER_DATA *);

static j2k_IComponent ComponentInterface = { &Component_getWidth,
                                             &Component_getHeight,
                                             &Component_getPrecision,
                                             &Component_isSigned,
                                             &Component_getOffsetX,
                                             &Component_getOffsetY,
                                             &Component_getSeparationX,
                                             &Component_getSeparationY,
                                             &Component_destruct};

J2KPRIV( uint32_t)
Component_getWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->width;
}

J2KPRIV( uint32_t)
Component_getHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->height;
}

J2KPRIV( uint32_t)
Component_getPrecision(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->precision;
}

J2KPRIV( NRT_BOOL)
Component_isSigned(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->isSigned;
}

J2KPRIV( int32_t)
Component_getOffsetX(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->x0;
}

J2KPRIV( int32_t)
Component_getOffsetY(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->y0;
}

J2KPRIV( uint32_t)
Component_getSeparationX(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->xSeparation;
}

J2KPRIV( uint32_t)
Component_getSeparationY(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ComponentImpl *impl = (ComponentImpl*) data;
    return impl->ySeparation;
}

J2KPRIV(void)
Component_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        J2K_FREE(data);
    }
}


J2KAPI(j2k_Component*) j2k_Component_construct(uint32_t width,
                                               uint32_t height,
                                               uint32_t precision,
                                               NRT_BOOL isSigned,
                                               uint32_t offsetX,
                                               uint32_t offsetY,
                                               uint32_t separationX,
                                               uint32_t separationY,
                                               nrt_Error *error)
{
    j2k_Component *component = NULL;
    ComponentImpl *impl = NULL;

    component = (j2k_Component*) J2K_MALLOC(sizeof(j2k_Component));
    if (!component)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(component, 0, sizeof(j2k_Component));

    /* create the Component interface */
    impl = (ComponentImpl *) J2K_MALLOC(sizeof(ComponentImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(ComponentImpl));
    component->data = impl;
    component->iface = &ComponentInterface;

    impl->width = width;
    impl->height = height;
    impl->precision = precision;
    impl->isSigned = isSigned;
    impl->x0 = offsetX;
    impl->y0 = offsetY;
    impl->xSeparation = separationX;
    impl->ySeparation = separationY;

    return component;

    CATCH_ERROR:
    {
        if (component)
        {
            j2k_Component_destruct(&component);
        }
        return NULL;
    }
}

