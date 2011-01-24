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

J2K_CXX_GUARD

typedef nrt_Uint32  (*J2K_ICONTAINER_GET_TILESX)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_TILESY)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_TILE_WIDTH)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_TILE_HEIGHT)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_WIDTH)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_HEIGHT)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_NUM_COMPONENTS)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_COMPONENT_BYTES)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICONTAINER_GET_COMPONENT_BITS)(J2K_USER_DATA*, nrt_Error*);
typedef int         (*J2K_ICONTAINER_GET_IMAGE_TYPE)(J2K_USER_DATA*, nrt_Error*);
typedef NRT_BOOL    (*J2K_ICONTAINER_IS_SIGNED)(J2K_USER_DATA*, nrt_Error*);
typedef void        (*J2K_ICONTAINER_DESTRUCT)(J2K_USER_DATA *);

typedef struct _j2k_IContainer
{
    J2K_ICONTAINER_GET_TILESX getTilesX;
    J2K_ICONTAINER_GET_TILESY getTilesY;
    J2K_ICONTAINER_GET_TILE_WIDTH getTileWidth;
    J2K_ICONTAINER_GET_TILE_HEIGHT getTileHeight;
    J2K_ICONTAINER_GET_WIDTH getWidth;
    J2K_ICONTAINER_GET_HEIGHT getHeight;
    J2K_ICONTAINER_GET_NUM_COMPONENTS getNumComponents;
    J2K_ICONTAINER_GET_COMPONENT_BYTES getComponentBytes;
    J2K_ICONTAINER_GET_COMPONENT_BITS getComponentBits;
    J2K_ICONTAINER_GET_IMAGE_TYPE getImageType;
    J2K_ICONTAINER_IS_SIGNED isSigned;
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
J2KAPI(j2k_Container*) j2k_Container_construct(nrt_Uint32 width,
                                               nrt_Uint32 height,
                                               nrt_Uint32 bands,
                                               nrt_Uint32 actualBitsPerPixel,
                                               nrt_Uint32 tileWidth,
                                               nrt_Uint32 tileHeight,
                                               int imageType,
                                               int isSigned,
                                               nrt_Error *error);

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
 * Returns the overall image width
 */
J2KAPI(nrt_Uint32) j2k_Container_getWidth(j2k_Container*, nrt_Error*);

/**
 * Returns the overall image height
 */
J2KAPI(nrt_Uint32) j2k_Container_getHeight(j2k_Container*, nrt_Error*);

/**
 * Returns the number of components/bands
 */
J2KAPI(nrt_Uint32) j2k_Container_getNumComponents(j2k_Container*, nrt_Error*);

/**
 * Returns the number of (expanded) bytes per sample per component
 */
J2KAPI(nrt_Uint32) j2k_Container_getComponentBytes(j2k_Container*, nrt_Error*);

/**
 * Returns the number of relevant bits per sample per component
 */
J2KAPI(nrt_Uint32) j2k_Container_getComponentBits(j2k_Container*, nrt_Error*);

/**
 * Returns the image type
 */
J2KAPI(int) j2k_Container_getImageType(j2k_Container*, nrt_Error*);

/**
 * Returns whether the data is signed
 */
J2KAPI(NRT_BOOL) j2k_Container_isSigned(j2k_Container*, nrt_Error*);

/**
 * Destroys the Container
 */
J2KAPI(void) j2k_Container_destruct(j2k_Container**);

J2K_CXX_ENDGUARD

#endif
