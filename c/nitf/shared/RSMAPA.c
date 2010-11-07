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

#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 80, "Image Identifier", "IID" },
    {NITF_BCS_A, 40, "RSM Image Support Data Edition", "EDITION" },
    {NITF_BCS_A, 40, "Triangulation ID", "TID" },

    {NITF_BCS_N, 2, "Number of Parameters", "NPAR" },

    {NITF_BCS_A, 21, "Local Coord Origin (XUOL)", "XUOL" },
    {NITF_BCS_A, 21, "Local Coord Origin (YUOL)", "YUOL" },
    {NITF_BCS_A, 21, "Local Coord Origin (ZUOL)", "ZUOL" },

    {NITF_BCS_A, 21, "Local Coord Unit Vector (XUXL)", "XUXL" },
    {NITF_BCS_A, 21, "Local Coord Unit Vector (XUYL)", "XUYL" },
    {NITF_BCS_A, 21, "Local Coord Unit Vector (XUZL)", "XUZL" },

    {NITF_BCS_A, 21, "Local Coord Unit Vector (YUXL)", "YUXL" },
    {NITF_BCS_A, 21, "Local Coord Unit Vector (YUYL)", "YUYL" },
    {NITF_BCS_A, 21, "Local Coord Unit Vector (YUZL)", "YUZL" },

    {NITF_BCS_A, 21, "Local Coord Unit Vector (ZUXL)", "ZUXL" },
    {NITF_BCS_A, 21, "Local Coord Unit Vector (ZUYL)", "ZUYL" },
    {NITF_BCS_A, 21, "Local Coord Unit Vector (ZUZL)", "ZUZL" },

    {NITF_BCS_A, 2, "Image Row Constant Index", "IR0" },
    {NITF_BCS_A, 2, "Image Row X Index", "IRX" },
    {NITF_BCS_A, 2, "Image Row Y Index", "IRY" },
    {NITF_BCS_A, 2, "Image Row Z Index", "IRZ" },

    {NITF_BCS_A, 2, "Image Row X^2 Index", "IRXX" },
    {NITF_BCS_A, 2, "Image Row XY Index", "IRXY" },
    {NITF_BCS_A, 2, "Image Row XZ Index", "IRXZ" },


    {NITF_BCS_A, 2, "Image Row Y^2 Index", "IRYY" },
    {NITF_BCS_A, 2, "Image Row YZ Index", "IRYZ" },

    {NITF_BCS_A, 2, "Image Row Z^2 Index", "IRZZ" },



    {NITF_BCS_A, 2, "Image Col Constant Index", "IC0" },
    {NITF_BCS_A, 2, "Image Col X Index", "ICX" },
    {NITF_BCS_A, 2, "Image Col Y Index", "ICY" },
    {NITF_BCS_A, 2, "Image Col Z Index", "ICZ" },

    {NITF_BCS_A, 2, "Image Col X^2 Index", "ICXX" },
    {NITF_BCS_A, 2, "Image Col XY Index", "ICXY" },
    {NITF_BCS_A, 2, "Image Col XZ Index", "ICXZ" },


    {NITF_BCS_A, 2, "Image Col Y^2 Index", "ICYY" },
    {NITF_BCS_A, 2, "Image Col YZ Index", "ICYZ" },

    {NITF_BCS_A, 2, "Image Col Z^2 Index", "ICZZ" },

    {NITF_BCS_A, 2, "Ground X Constant Index", "GX0" },
    {NITF_BCS_A, 2, "Ground Y Constant Index", "GY0" },
    {NITF_BCS_A, 2, "Ground Z Constant Index", "GZ0" },

    {NITF_BCS_A, 2, "Ground Rotation X", "GXR" },
    {NITF_BCS_A, 2, "Ground Rotation Y", "GYR" },
    {NITF_BCS_A, 2, "Ground Rotation Z", "GZR" },

    {NITF_BCS_A, 2, "Ground Scale", "GS" },

    {NITF_BCS_A, 2, "Ground X Adj Proportional to X index", "GXX" },
    {NITF_BCS_A, 2, "Ground X Adj Proportional to Y index", "GXY" },
    {NITF_BCS_A, 2, "Ground X Adj Proportional to Z index", "GXZ" },


    {NITF_BCS_A, 2, "Ground Y Adj Proportional to X index", "GYX" },
    {NITF_BCS_A, 2, "Ground Y Adj Proportional to Y index", "GYY" },
    {NITF_BCS_A, 2, "Ground Y Adj Proportional to Z index", "GYZ" },

    {NITF_BCS_A, 2, "Ground Z Adj Proportional to X index", "GZX" },
    {NITF_BCS_A, 2, "Ground Z Adj Proportional to Y index", "GZY" },
    {NITF_BCS_A, 2, "Ground Z Adj Proportional to Z index", "GZZ" },

    
    {NITF_LOOP, 0, NULL, "NPAR" },

    {NITF_BCS_A, 21, "Component Value", "PARVAL" },

    {NITF_ENDLOOP, 0, NULL, NULL},


    {NITF_END, 0, NULL, NULL}

    
};



NITF_DECLARE_SINGLE_PLUGIN(RSMAPA, description)

NITF_CXX_ENDGUARD
