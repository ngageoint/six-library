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

#ifndef __J2K_COMPONENT_H__
#define __J2K_COMPONENT_H__

#include "j2k/Defines.h"

J2K_CXX_GUARD

typedef nrt_Uint32  (*J2K_ICOMPONENT_GET_WIDTH)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICOMPONENT_GET_HEIGHT)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICOMPONENT_GET_PRECISION)(J2K_USER_DATA*, nrt_Error*);
typedef NRT_BOOL    (*J2K_ICOMPONENT_IS_SIGNED)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Int32   (*J2K_ICOMPONENT_GET_OFFSET_X)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Int32   (*J2K_ICOMPONENT_GET_OFFSET_Y)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICOMPONENT_GET_SEPARATION_X)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32  (*J2K_ICOMPONENT_GET_SEPARATION_Y)(J2K_USER_DATA*, nrt_Error*);
typedef void        (*J2K_ICOMPONENT_DESTRUCT)(J2K_USER_DATA *);

typedef struct _j2k_IComponent
{
    J2K_ICOMPONENT_GET_WIDTH getWidth;
    J2K_ICOMPONENT_GET_HEIGHT getHeight;
    J2K_ICOMPONENT_GET_PRECISION getPrecision;
    J2K_ICOMPONENT_IS_SIGNED isSigned;
    J2K_ICOMPONENT_GET_OFFSET_X getOffsetX;
    J2K_ICOMPONENT_GET_OFFSET_Y getOffsetY;
    J2K_ICOMPONENT_GET_SEPARATION_X getSeparationX;
    J2K_ICOMPONENT_GET_SEPARATION_Y getSeparationY;
    J2K_ICOMPONENT_DESTRUCT destruct;
} j2k_IComponent;

typedef struct _j2k_Component
{
    j2k_IComponent *iface;
    J2K_USER_DATA *data;
} j2k_Component;

/**
 * Constructs a Component from scratch using the provided metadata
 */
J2KAPI(j2k_Component*) j2k_Component_construct(nrt_Uint32 width,
                                               nrt_Uint32 height,
                                               nrt_Uint32 precision,
                                               NRT_BOOL isSigned,
                                               nrt_Uint32 offsetX,
                                               nrt_Uint32 offsetY,
                                               nrt_Uint32 separationX,
                                               nrt_Uint32 separationY,
                                               nrt_Error *error);

/**
 * Returns the width of the Component
 */
J2KAPI(nrt_Uint32) j2k_Component_getWidth(j2k_Component*, nrt_Error*);

/**
 * Returns the height of the Component
 */
J2KAPI(nrt_Uint32) j2k_Component_getHeight(j2k_Component*, nrt_Error*);

/**
 * Returns the number of bits per sample per component
 */
J2KAPI(nrt_Uint32) j2k_Component_getPrecision(j2k_Component*, nrt_Error*);

/**
 * Returns whether the data is signed
 */
J2KAPI(NRT_BOOL) j2k_Component_isSigned(j2k_Component*, nrt_Error*);

/**
 * Returns the X offset into the reference grid
 */
J2KAPI(nrt_Int32) j2k_Component_getOffsetX(j2k_Component*, nrt_Error*);

/**
 * Returns the Y offset into the reference grid
 */
J2KAPI(nrt_Int32) j2k_Component_getOffsetY(j2k_Component*, nrt_Error*);

/**
 * Returns the width separation between samples on the reference grid
 */
J2KAPI(nrt_Uint32) j2k_Component_getSeparationX(j2k_Component*, nrt_Error*);

/**
 * Returns the height separation between samples on the reference grid
 */
J2KAPI(nrt_Uint32) j2k_Component_getSeparationY(j2k_Component*, nrt_Error*);

/**
 * Destroys the Component
 */
J2KAPI(void) j2k_Component_destruct(j2k_Component**);

J2K_CXX_ENDGUARD

#endif
