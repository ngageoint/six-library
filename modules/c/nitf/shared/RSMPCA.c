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

    {NITF_BCS_N, 3, "Row Section Number", "RSN" },
    {NITF_BCS_N, 3, "Column Section Number", "CSN" },

    {NITF_BCS_A, 21, "Row Fitting Error", "RFEP" },
    {NITF_BCS_A, 21, "Col Fitting Error", "CFEP" },

    {NITF_BCS_A, 21, "Row Normalization Offset", "RNRMO" },
    {NITF_BCS_A, 21, "Col Normalization Offset", "CNRMO" },

    {NITF_BCS_A, 21, "X Normalization Offset", "XNRMO" },
    {NITF_BCS_A, 21, "Y Normalization Offset", "YNRMO" },
    {NITF_BCS_A, 21, "Z Normalization Offset", "ZNRMO" },

    {NITF_BCS_A, 21, "Row Normalization Scale Factor", "RNRMSF" },
    {NITF_BCS_A, 21, "Col Normalization Scale Factor", "CNRMSF" },

    {NITF_BCS_A, 21, "X Normalization Scale Factor", "XNRMSF" },
    {NITF_BCS_A, 21, "Y Normalization Scale Factor", "YNRMSF" },
    {NITF_BCS_A, 21, "Z Normalization Scale Factor", "ZNRMSF" },

    {NITF_BCS_N, 1, "Row Numerator Poly Max Power of X", "RNPWRX" },
    {NITF_BCS_N, 1, "Row Numerator Poly Max Power of Y", "RNPWRY" },
    {NITF_BCS_N, 1, "Row Numerator Poly Max Power of Z", "RNPWRZ" },

    {NITF_BCS_N, 3, "Row Numerator Poly Number of Poly Terms", "RNTRMS" },

    {NITF_LOOP, 0, NULL, "RNTRMS"},

    {NITF_BCS_A, 21, "Poly Coeff", "RNPCF" },

    {NITF_ENDLOOP, 0, NULL, NULL},


    {NITF_BCS_N, 1, "Row Denominator Poly Max Power of X", "RDPWRX" },
    {NITF_BCS_N, 1, "Row Denominator Poly Max Power of Y", "RDPWRY" },
    {NITF_BCS_N, 1, "Row Denominator Poly Max Power of Z", "RDPWRZ" },

    {NITF_BCS_N, 3, "Row Denominator Poly Number of Poly Terms", "RDTRMS" },
    
    {NITF_LOOP, 0, NULL, "RDTRMS"},

    {NITF_BCS_A, 21, "Poly Coeff", "RDPCF" },

    {NITF_ENDLOOP, 0, NULL, NULL},



    {NITF_BCS_N, 1, "Row Numerator Poly Max Power of X", "CNPWRX" },
    {NITF_BCS_N, 1, "Row Numerator Poly Max Power of Y", "CNPWRY" },
    {NITF_BCS_N, 1, "Row Numerator Poly Max Power of Z", "CNPWRZ" },

    {NITF_BCS_N, 3, "Row Numerator Poly Number of Poly Terms", "CNTRMS" },

    {NITF_LOOP, 0, NULL, "CNTRMS"},

    {NITF_BCS_A, 21, "Poly Coeff", "CNPCF" },

    {NITF_ENDLOOP, 0, NULL, NULL},


    {NITF_BCS_N, 1, "Row Denominator Poly Max Power of X", "CDPWRX" },
    {NITF_BCS_N, 1, "Row Denominator Poly Max Power of Y", "CDPWRY" },
    {NITF_BCS_N, 1, "Row Denominator Poly Max Power of Z", "CDPWRZ" },

    {NITF_BCS_N, 3, "Row Denominator Poly Number of Poly Terms", "CDTRMS" },
    
    {NITF_LOOP, 0, NULL, "CDTRMS"},

    {NITF_BCS_A, 21, "Poly Coeff", "CDPCF" },

    {NITF_ENDLOOP, 0, NULL, NULL},

    {NITF_END, 0, NULL, NULL}

    
};

NITF_DECLARE_SINGLE_PLUGIN(RSMPCA, description)

NITF_CXX_ENDGUARD
