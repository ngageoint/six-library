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

    {NITF_BCS_A, 21, "Low Order Poly Const Coeff for Row", "GR0" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of X for Row", "GRX" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of Y for Row", "GRY" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of Z for Row", "GRZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coeff of XX for Row", "GRXX" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of XY for Row", "GRXY" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of XZ for Row", "GRXZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coeff of YY for Row", "GRYY" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of YZ for Row", "GRYZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coeff of ZZ for Row", "GRZZ" },


    {NITF_BCS_A, 21, "Low Order Poly Const Coeff for Col", "GC0" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of X for Col", "GCX" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of Y for Col", "GCY" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of Z for Col", "GCZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coeff of XX for Col", "GCXX" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of XY for Col", "GCXY" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of XZ for Col", "GCXZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coeff of YY for Col", "GCYY" },
    {NITF_BCS_A, 21, "Low Order Poly Coeff of YZ for Col", "GCYZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coeff of ZZ for Col", "GCZZ" },

    {NITF_BCS_N, 3, "Row Number of Image Sections", "GRNIS" },
    {NITF_BCS_N, 3, "Col Number of Image Sections", "GCNIS" },
    {NITF_BCS_N, 3, "Total Number of Image Sections", "GTNIS" },

    {NITF_BCS_A, 21, "Section Size in Rows", "GRSSIZ" },
    {NITF_BCS_A, 21, "Section Size in Cols", "GCSSIZ" },

    {NITF_END, 0, NULL, NULL}

    
};



NITF_DECLARE_SINGLE_PLUGIN(RSMGIA, description)

NITF_CXX_ENDGUARD
