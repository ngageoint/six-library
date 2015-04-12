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

static int nxptsXnypts(nitf_TRE* tre, char idx[10][10], int depth, nitf_Error* error)
{
    int nxpts, nypts;
    char fname[64];
    nitf_Field* field;
    strcpy(fname, "NXPTS");
    strcat(fname, idx[0]);
    field = nitf_TRE_getField(tre, fname);
    nitf_Field_get(field, &nxpts, NITF_CONV_INT, sizeof(nxpts), error);

    strcpy(fname, "NYPTS");
    strcat(fname, idx[0]);
    field = nitf_TRE_getField(tre, fname);
    nitf_Field_get(field, &nypts, NITF_CONV_INT, sizeof(nypts), error);

    return nxpts * nypts;
}

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 80, "Image Identifier", "IID" },
    {NITF_BCS_A, 40, "RSM Image Support Data Edition", "EDITION" },

    {NITF_BCS_N, 3, "Ground-to-image Grid Row Section Number", "GGRSN" },
    {NITF_BCS_N, 3, "Ground-to-image Grid Col Section Number", "GGCSN" },


    {NITF_BCS_A, 21, "Ground-to-image Grid Row Fitting Error", "GGRFEP" },
    {NITF_BCS_A, 21, "Ground-to-image Grid Col Fitting Error", "GGCFEP" },

    {NITF_BCS_A, 1, "Ground-to-image Grid Interp Order", "INTORD" },

    {NITF_BCS_N, 3, "Number of Grid Planes", "NPLN" },
    {NITF_BCS_A, 21, "Delta Z Between Grid Planes", "DELTAZ" },
    {NITF_BCS_A, 21, "Delta X Between Grid Points", "DELTAX" },
    {NITF_BCS_A, 21, "Delta Y Between Grid Points", "DELTAY" },

    {NITF_BCS_A, 21, "Z Value of Plane 1", "ZPLN1" },
    {NITF_BCS_A, 21, "X Value of Initial Point in Plane 1", "XIPLN1" },
    {NITF_BCS_A, 21, "Y Value of Initial Point in Plane 1", "YIPLN1" },

    {NITF_BCS_N, 9, "Ref Image Row Coord Value", "REFROW" },
    {NITF_BCS_N, 9, "Ref Image Col Coord Value", "REFCOL" },

    {NITF_BCS_N, 2, "Total Num of Image Row Coord Digits", "TNUMRD" },
    {NITF_BCS_N, 2, "Total Num of Image Col Coord Digits", "TNUMCD" },

    {NITF_BCS_N, 1, "Num of Image Row Coord Fractional Digits", "FNUMRD" },
    {NITF_BCS_N, 1, "Num of Image Col Coord Fractional Digits", "FNUMCD" },

    /* This [0] value corresponds to second grid plan */
    {NITF_LOOP, 0, "- 1", "NPLN"},

        {NITF_BCS_N, 4, "Initial Grid Points X Off", "IXO" },
        {NITF_BCS_N, 4, "Initial Grid Points Y Off", "IYO" },

    {NITF_ENDLOOP, 0, NULL, NULL },

    {NITF_LOOP, 0, NULL, "NPLN"},

        {NITF_BCS_N, 3, "Num of Grid Points in the X Direction", "NXPTS" },
        {NITF_BCS_N, 3, "Num of Grid Points in the Y Direction", "NYPTS" },

        {NITF_LOOP, 0, NITF_FUNCTION, (char*)&nxptsXnypts},

            {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Grid Point's Row Coord",
                    "RCOORD", "TNUMRD" },
            {NITF_BCS_A, NITF_TRE_CONDITIONAL_LENGTH, "Grid Point's Col Coord",
                    "CCOORD", "TNUMCD" },

        {NITF_ENDLOOP, 0, NULL, NULL }, /* NXPTS x NYPTS */
    {NITF_ENDLOOP, 0, NULL, NULL }, /* NPLN */

    {NITF_END, 0, NULL, NULL}

};



NITF_DECLARE_SINGLE_PLUGIN(RSMGGA, description)

NITF_CXX_ENDGUARD
