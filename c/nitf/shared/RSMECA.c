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

static int numopg(nitf_TRE* tre, char idx[10][10], int depth, nitf_Error* error)
{
    nitf_Field* field;
    int numopg, x;
    char fname[64];
    strcpy(fname, "NUMOPG");
    x = strlen(idx[0]);
    
    strcat(fname, idx[0]);

    field = nitf_TRE_getField(tre, fname);
    nitf_Field_get(field, &numopg, NITF_CONV_INT, sizeof(numopg), error);
    return ( ((numopg + 1) * numopg) / 2 );

}


static int mapped(nitf_TRE* tre, char idx[10][10], int depth, nitf_Error* error)
{
    int npar, npar0;
    nitf_Field* field = nitf_TRE_getField(tre, "NPAR");
    nitf_Field_get(field, &npar, NITF_CONV_INT, sizeof(npar), error);

    field = nitf_TRE_getField(tre, "NPAR0");
    nitf_Field_get(field, &npar0, NITF_CONV_INT, sizeof(npar0), error);
    return ( npar * npar0 );

}

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 80, "Image Identifier", "IID" },
    {NITF_BCS_A, 40, "RSM Image Support Data Edition", "EDITION" },
    {NITF_BCS_A, 40, "Triangulation ID", "TID" },

    {NITF_BCS_A, 1, "Include Indirect Error Covariance", "INCLIC" },
    {NITF_BCS_A, 1, "Include Indirect Error Covariance", "INCLUC" },

   /*  {NITF_IF, 0, NITF_FUNCTION, &inclic }, */
    
    {NITF_IF, 0, "eq Y", "INCLIC"},
    {NITF_BCS_N, 2, "Number of Parameters", "NPAR" },
    {NITF_BCS_N, 2, "Number of Original Adjustable Params", "NPAR0" },
    {NITF_BCS_N, 2, "Number of Independent Subgroups", "IGN" },

    {NITF_BCS_A, 8, "Version Date of the Orig Image Err Cov", "CVDATE"},
    

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


    {NITF_LOOP, 0, NULL, "IGN" },

    {NITF_BCS_N, 2, "Number of Orig Adj Params in Subgroup", "NUMOPG" },

    {NITF_LOOP, 0, NITF_FUNCTION, (char*)&numopg},

    {NITF_BCS_A, 21, "Original Error Covariance Element", "ERRCVG" },

    {NITF_ENDLOOP, 0, NULL, NULL},
    
    {NITF_BCS_N, 1, "Time Correlation Domain Flag", "TCDF" },
    {NITF_BCS_N, 1, "Number of Correlation Segments", "NCSEG" },

    {NITF_LOOP, 0, NULL, "NCSEG" },
    
    {NITF_BCS_A, 21, "Segment Correlation Value", "CORSEG" },
    {NITF_BCS_A, 21, "Segment Tau Value", "TAUSEG" },

    {NITF_ENDLOOP, 0, NULL, NULL}, /* NCSEG */

    {NITF_ENDLOOP, 0, NULL, NULL}, /* IGN */


    {NITF_LOOP, 0, NITF_FUNCTION, (char*)&mapped},
    {NITF_BCS_A, 21, "Mapping Matrix Element", "MAP" },
    {NITF_ENDLOOP, 0, NULL, NULL}, /* MAP */

    {NITF_ENDIF, 0, NULL, NULL },
    
    {NITF_IF, 0, "eq Y", "INCLUC"},

    {NITF_BCS_A, 21, "Unmodeled Row Variance", "URR" },
    {NITF_BCS_A, 21, "Unmodeled Row-Column Covariance", "URC" },
    {NITF_BCS_A, 21, "Unmodeled Column Variance", "UCC" },
    {NITF_BCS_N, 1, "Number of Corr Segs for ind-variable Row distance", "UNCSR" },
    
    {NITF_LOOP, 0, NULL, "UNCSR" },

    {NITF_BCS_A, 21, "Segment Correlation Value", "UCORSR" },
    {NITF_BCS_A, 21, "Segment Tau Value", "UTAUSR" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_BCS_N, 1, "Number of Corr Segs for ind-variable Col distance", "UNCSC" },

    {NITF_LOOP, 0, NULL, "UNCSC" },

    {NITF_BCS_A, 21, "Segment Correlation Value", "UCORSC" },
    {NITF_BCS_A, 21, "Segment Tau Value", "UTAUSC" },

    {NITF_ENDLOOP, 0, NULL, NULL},
    
    {NITF_ENDIF, 0, NULL, NULL },

    {NITF_END, 0, NULL, NULL}

    
};



NITF_DECLARE_SINGLE_PLUGIN(RSMECA, description)

NITF_CXX_ENDGUARD
