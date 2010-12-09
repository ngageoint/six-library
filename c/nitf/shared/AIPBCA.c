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
    {NITF_BCS_N, 5, "Patch Width", "Patch_Width", },
    {NITF_BCS_A, 16, "Semi-major axis (x component)", "u_hat_x" },
    {NITF_BCS_A, 16, "Semi-major axis (y component)", "u_hat_y" },
    {NITF_BCS_A, 16, "Semi-major axis (z component)", "u_hat_z" },

    {NITF_BCS_A, 16, "Semi-minor axis (x component)", "v_hat_x" },
    {NITF_BCS_A, 16, "Semi-minor axis (y component)", "v_hat_y" },
    {NITF_BCS_A, 16, "Semi-minor axis (z component)", "v_hat_z" },

    {NITF_BCS_A, 16, "Normal vector (x component)", "n_hat_x" },
    {NITF_BCS_A, 16, "Normal vector (y component)", "n_hat_y" },
    {NITF_BCS_A, 16, "Normal vector (z component)", "n_hat_z" },

    {NITF_BCS_N, 7, "Cross-track depression angle", "Dep_Angle" },
    {NITF_BCS_N, 10, "Cross-track range", "CT_Track_Range" },
    
    {NITF_BCS_A, 16, "Dummy param, leading edge of patch", "eta_0" },
    {NITF_BCS_A, 16, "Dummy param, trailing edge of patch", "eta_1" },

    {NITF_BCS_A, 9, "Component u of image coords (x direction)", "x_Img_u" },
    {NITF_BCS_A, 9, "Component v of image coords (x direction)", "x_Img_v" },
    {NITF_BCS_A, 9, "Component n of image coords (x direction)", "x_Img_n" },

    {NITF_BCS_A, 9, "Component u of image coords (y direction)", "y_Img_u" },
    {NITF_BCS_A, 9, "Component v of image coords (y direction)", "y_Img_v" },
    {NITF_BCS_A, 9, "Component n of image coords (y direction)", "y_Img_n" },

    {NITF_BCS_A, 9, "Component u of image coords (z direction)", "z_Img_u" },
    {NITF_BCS_A, 9, "Component v of image coords (z direction)", "z_Img_v" },
    {NITF_BCS_A, 9, "Component n of image coords (z direction)", "z_Img_n" },

    {NITF_BCS_A, 9, "Cross-track unit vector (x component)", "ct_hat_x" },
    {NITF_BCS_A, 9, "Cross-track unit vector (y component)", "ct_hat_y" },
    {NITF_BCS_A, 9, "Cross-track unit vector (z component)", "ct_hat_z" },

    {NITF_BCS_A, 13, "Scene center line point (u component)", "scl_pt_u" },
    {NITF_BCS_A, 13, "Scene center line point (v component)", "scl_pt_v" },
    {NITF_BCS_A, 13, "Scene center line point (n component)", "scl_pt_n" },

    {NITF_BCS_A, 13, "Semi-major axis length", "sigma_sm" },
    {NITF_BCS_A, 13, "Semi-minor axis length", "sigma_sn" },

    {NITF_BCS_A, 10, "Small circle origin offset (along v)", "s_off" },
    {NITF_BCS_A, 12, "Offset btwn small and great circle plane", "Rn_offset" },

    {NITF_BCS_N, 11, "Range to CRP", "CRP_Range" },    
    {NITF_BCS_N, 7, "Scene ref. depression angle", "Ref_Dep_Ang" },
    {NITF_BCS_N, 9, "Scene ref. aspect angle", "Ref_Asp_Ang" },
    {NITF_BCS_N, 1, "N-skip azimuth", "n_Skip_Az" },
    {NITF_BCS_N, 1, "N-skip range", "n_Skip_Range" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(AIPBCA, description)

NITF_CXX_ENDGUARD
