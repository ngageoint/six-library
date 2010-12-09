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

/*
 * FIXME: these names look wrong
 */
#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 2, "Destination Point", "destp" },
    {NITF_BCS_A, 3, "MTI Packet ID No.", "mtpid" },
    {NITF_BCS_A, 4, "Patch Number", "pchno" },
    {NITF_BCS_A, 5, "WAMTI Frame Number", "wamfn" },
    {NITF_BCS_A, 1, "WAMTI Bar Number", "wambn" },
    {NITF_BCS_A, 8, "UTC", "utc" },
    {NITF_BCS_A, 5, "Squint Angle", "sqnta" },
    {NITF_BCS_A, 7, "Cos of Grazing Angle", "cosgz" },
    {NITF_BCS_N, 3, "Number of Valid Targets", "nvtgt" },
    {NITF_LOOP, 0, NULL, "nvtgt"},
    {NITF_BCS_A, 21, "Target Location", "tgloc" },
    {NITF_BCS_A, 4, "Target Radial Velocity", "tgrdv" },
    {NITF_BCS_A, 3, "Target Estim Gnd Speed", "tggsp" },
    {NITF_BCS_A, 3, "Target Heading", "tghea" },
    {NITF_BCS_A, 2, "Target Signal Amplitude", "tgsig" },
    {NITF_BCS_A, 1, "Target Category", "tgcat" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(MTIRPA, description)

NITF_CXX_ENDGUARD
