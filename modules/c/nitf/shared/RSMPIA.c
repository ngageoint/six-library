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

    {NITF_BCS_A, 21, "Low Order Poly Const Coeff for Row", "R0" },

    {NITF_BCS_A, 21, "Low Order Poly Coff of X for Row", "RX" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of Y for Row", "RY" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of Z for Row", "RZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coff of XX for Row", "RXX" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of XY for Row", "RXY" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of XZ for Row", "RXZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coff of YY for Row", "RYY" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of YZ for Row", "RYZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coff of ZZ for Row", "RZZ" },

    {NITF_BCS_A, 21, "Low Order Poly Const Coeff for Col", "C0" },

    {NITF_BCS_A, 21, "Low Order Poly Coff of X for Col", "CX" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of Y for Col", "CY" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of Z for Col", "CZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coff of XX for Col", "CXX" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of XY for Col", "CXY" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of XZ for Col", "CXZ" },

    {NITF_BCS_A, 21, "Low Order Poly Coff of YY for Col", "CYY" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of YZ for Col", "CYZ" },
    {NITF_BCS_A, 21, "Low Order Poly Coff of ZZ for Col", "CZZ" },

    {NITF_BCS_N, 3, "Row Number of Image Sections", "RNIS" },
    {NITF_BCS_N, 3, "Column Number of Image Sections", "CNIS" },
    {NITF_BCS_N, 3, "Total Number of Image Sections", "TNIS" },

    {NITF_BCS_A, 21, "Section Size in Rows", "RSSIZ" },
    {NITF_BCS_A, 21, "Section Size in Cols", "CSSIZ" },

    {NITF_END, 0, NULL, NULL}    
};

NITF_DECLARE_SINGLE_PLUGIN(RSMPIA, description)

NITF_CXX_ENDGUARD
