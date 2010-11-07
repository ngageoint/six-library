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
    {NITF_BCS_A, 28, "Last Name", "LASTNME" },
    {NITF_BCS_A, 28, "First Name", "FIRSTNME" },
    {NITF_BCS_A, 28, "Middle Name", "MIDNME" },
    {NITF_BCS_A, 6, "Birth Date", "DOB" },
    {NITF_BCS_A, 2, "Associated Country", "ASSOCTRY" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(PIAPEA, description)

NITF_CXX_ENDGUARD
