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

#include "j2k/Container.h"


typedef struct _ContainerImpl
{
    uint32_t gridWidth;
    uint32_t gridHeight;
    uint32_t nComponents;
    j2k_Component **components;
    uint32_t xTiles;
    uint32_t yTiles;
    uint32_t tileWidth;
    uint32_t tileHeight;
    int imageType;
} ContainerImpl;


J2KPRIV( uint32_t)    Container_getGridWidth(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t)    Container_getGridHeight(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t)    Container_getNumComponents(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( j2k_Component*)Container_getComponent(J2K_USER_DATA *, uint32_t, nrt_Error *);
J2KPRIV( uint32_t)    Container_getTilesX(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t)    Container_getTilesY(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t)    Container_getTileWidth(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( uint32_t)    Container_getTileHeight(J2K_USER_DATA *, nrt_Error *);
J2KPRIV( int)           Container_getImageType(J2K_USER_DATA *, nrt_Error *);
J2KPRIV(void)           Container_destruct(J2K_USER_DATA *);

static j2k_IContainer ContainerInterface = { &Container_getGridWidth,
                                             &Container_getGridHeight,
                                             &Container_getNumComponents,
                                             &Container_getComponent,
                                             &Container_getTilesX,
                                             &Container_getTilesY,
                                             &Container_getTileWidth,
                                             &Container_getTileHeight,
                                             &Container_getImageType,
                                             &Container_destruct};

J2KPRIV( uint32_t)
Container_getGridWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->gridWidth;
}

J2KPRIV( uint32_t)
Container_getGridHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->gridHeight;
}

J2KPRIV( uint32_t)
Container_getNumComponents(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->nComponents;
}

J2KPRIV( j2k_Component*)
Container_getComponent(J2K_USER_DATA *data, uint32_t idx, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    if (idx >= impl->nComponents)
    {
        nrt_Error_init(error, "Invalid component index",
                       NRT_CTXT, NRT_ERR_INVALID_OBJECT);
        return NULL;
    }
    return impl->components[idx];
}

J2KPRIV( uint32_t)
Container_getTilesX(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->xTiles;
}

J2KPRIV( uint32_t)
Container_getTilesY(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->yTiles;
}

J2KPRIV( uint32_t)
Container_getTileWidth(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->tileWidth;
}

J2KPRIV( uint32_t)
Container_getTileHeight(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->tileHeight;
}

J2KPRIV( int)
Container_getImageType(J2K_USER_DATA *data, nrt_Error *error)
{
    (void)error;

    ContainerImpl *impl = (ContainerImpl*) data;
    return impl->imageType;
}

J2KPRIV(void)
Container_destruct(J2K_USER_DATA * data)
{
    if (data)
    {
        ContainerImpl *impl = (ContainerImpl*) data;
        if (impl->components)
        {
            uint32_t i = 0;
            for(; i < impl->nComponents; ++i)
            {
                j2k_Component *c = impl->components[i];
                if (c)
                    j2k_Component_destruct(&c);
            }
            J2K_FREE(impl->components);
            impl->components = NULL;
        }
        J2K_FREE(data);
    }
}


J2KAPI(j2k_Container*) j2k_Container_construct(uint32_t gridWidth,
                                               uint32_t gridHeight,
                                               uint32_t numComponents,
                                               j2k_Component** components,
                                               uint32_t tileWidth,
                                               uint32_t tileHeight,
                                               int imageType,
                                               nrt_Error *error)
{
    j2k_Container *container = NULL;
    ContainerImpl *impl = NULL;

    container = (j2k_Container*) J2K_MALLOC(sizeof(j2k_Container));
    if (!container)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(container, 0, sizeof(j2k_Container));

    /* create the Container interface */
    impl = (ContainerImpl *) J2K_MALLOC(sizeof(ContainerImpl));
    if (!impl)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT, NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(impl, 0, sizeof(ContainerImpl));
    container->data = impl;
    container->iface = &ContainerInterface;

    impl->gridWidth = gridWidth;
    impl->gridHeight = gridHeight;
    impl->nComponents = numComponents;
    impl->components = components;
    impl->tileWidth = tileWidth;
    impl->tileHeight = tileHeight;
    impl->xTiles = gridWidth / tileWidth + (gridWidth % tileWidth == 0 ? 0 : 1);
    impl->yTiles = gridHeight / tileHeight + (gridHeight % tileHeight == 0 ? 0 : 1);
    impl->imageType = imageType;

    return container;

    CATCH_ERROR:
    {
        if (container)
        {
            j2k_Container_destruct(&container);
        }
        return NULL;
    }
}
