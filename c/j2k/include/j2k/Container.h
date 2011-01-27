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

#ifndef __J2K_CONTAINER_H__
#define __J2K_CONTAINER_H__

#include "j2k/Defines.h"
#include "j2k/Component.h"

J2K_CXX_GUARD

typedef nrt_Uint32      (*J2K_ICONTAINER_GET_GRID_WIDTH)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32      (*J2K_ICONTAINER_GET_GRID_HEIGHT)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32      (*J2K_ICONTAINER_GET_NUM_COMPONENTS)(J2K_USER_DATA*, nrt_Error*);
typedef j2k_Component*  (*J2K_ICONTAINER_GET_COMPONENT)(J2K_USER_DATA*, nrt_Uint32, nrt_Error*);
typedef nrt_Uint32      (*J2K_ICONTAINER_GET_TILESX)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32      (*J2K_ICONTAINER_GET_TILESY)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32      (*J2K_ICONTAINER_GET_TILE_WIDTH)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32      (*J2K_ICONTAINER_GET_TILE_HEIGHT)(J2K_USER_DATA*, nrt_Error*);
typedef int             (*J2K_ICONTAINER_GET_IMAGE_TYPE)(J2K_USER_DATA*, nrt_Error*);
typedef void            (*J2K_ICONTAINER_DESTRUCT)(J2K_USER_DATA *);

typedef struct _j2k_IContainer
{
    J2K_ICONTAINER_GET_GRID_WIDTH getGridWidth;
    J2K_ICONTAINER_GET_GRID_HEIGHT getGridHeight;
    J2K_ICONTAINER_GET_NUM_COMPONENTS getNumComponents;
    J2K_ICONTAINER_GET_COMPONENT getComponent;

    J2K_ICONTAINER_GET_TILESX getTilesX;
    J2K_ICONTAINER_GET_TILESY getTilesY;
    J2K_ICONTAINER_GET_TILE_WIDTH getTileWidth;
    J2K_ICONTAINER_GET_TILE_HEIGHT getTileHeight;

    J2K_ICONTAINER_GET_IMAGE_TYPE getImageType;
    J2K_ICONTAINER_DESTRUCT destruct;
} j2k_IContainer;

typedef struct _j2k_Container
{
    j2k_IContainer *iface;
    J2K_USER_DATA *data;
} j2k_Container;

/**
 * Constructs a Container from scratch using the provided metadata
 */
J2KAPI(j2k_Container*) j2k_Container_construct(nrt_Uint32 gridWidth,
                                               nrt_Uint32 gridHeight,
                                               nrt_Uint32 numComponents,
                                               j2k_Component** components,
                                               nrt_Uint32 tileWidth,
                                               nrt_Uint32 tileHeight,
                                               int imageType,
                                               nrt_Error *error);

/**
 * Returns the reference grid width
 */
J2KAPI(nrt_Uint32) j2k_Container_getGridWidth(j2k_Container*, nrt_Error*);

/**
 * Returns the reference grid height
 */
J2KAPI(nrt_Uint32) j2k_Container_getGridHeight(j2k_Container*, nrt_Error*);

/**
 * Returns the number of components/bands
 */
J2KAPI(nrt_Uint32) j2k_Container_getNumComponents(j2k_Container*, nrt_Error*);

/**
 * Returns the i'th Component
 */
J2KAPI(j2k_Component*) j2k_Container_getComponent(j2k_Container*, nrt_Uint32, nrt_Error*);

/**
 * Returns the actual image width (in samples) - i.e. the max component width
 */
J2KAPI(nrt_Uint32) j2k_Container_getWidth(j2k_Container*, nrt_Error*);

/**
 * Returns the actual image height (in samples) - i.e. the max component height
 */
J2KAPI(nrt_Uint32) j2k_Container_getHeight(j2k_Container*, nrt_Error*);

/**
 * Returns the max component precision
 */
J2KAPI(nrt_Uint32) j2k_Container_getPrecision(j2k_Container*, nrt_Error*);

/**
 * Returns the number of column tiles (X-direction)
 */
J2KAPI(nrt_Uint32) j2k_Container_getTilesX(j2k_Container*, nrt_Error*);

/**
 * Returns the number of row tiles (Y-direction)
 */
J2KAPI(nrt_Uint32) j2k_Container_getTilesY(j2k_Container*, nrt_Error*);

/**
 * Returns the tile width
 */
J2KAPI(nrt_Uint32) j2k_Container_getTileWidth(j2k_Container*, nrt_Error*);

/**
 * Returns the tile height
 */
J2KAPI(nrt_Uint32) j2k_Container_getTileHeight(j2k_Container*, nrt_Error*);

/**
 * Returns the image type
 */
J2KAPI(int) j2k_Container_getImageType(j2k_Container*, nrt_Error*);

/**
 * Destroys the Container
 */
J2KAPI(void) j2k_Container_destruct(j2k_Container**);

J2K_CXX_ENDGUARD

#endif
