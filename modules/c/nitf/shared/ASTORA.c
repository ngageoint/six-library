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

    {NITF_BCS_N, 6, "Number of rows in full image prod.", "IMG_TOTAL_ROWS" },
    {NITF_BCS_N, 6, "Number of columns in full image prod.", 
     "IMG_TOTAL_COLS" },

    {NITF_BCS_N, 6, "Upper left corner of tile, row coord.", "IMG_INDEX_ROW" },
    {NITF_BCS_N, 6, "Upper left corner of tile, col coord.", "IMG_INDEX_COL" },
    
    {NITF_BCS_A, 7, "Distance from ref. ellipsoid to MSL geoid at ref. pt.", 
     "GEOID_OFFSET" },

    {NITF_BCS_A, 16, "Cone angle (radians)", "ALPHA_0" },

    {NITF_BCS_N, 2, "Left/right look flag", "K_L" },  

    {NITF_BCS_A, 15, "Speed of light (m/s)", "C_M" },
  
    {NITF_BCS_A, 16, "Nominal Aircraft Roll (radians)", "AC_ROLL" },

    {NITF_BCS_A, 16, "Nominal Aircraft Pitch (radians)", "AC_PITCH" },

    {NITF_BCS_A, 16, "Nominal Aircraft Yaw (radians)", "AC_YAW" },

    {NITF_BCS_A, 16, "Nominal Aircraft Track Heading (radians)", 
     "AC_TRACK_HEADING" 
    },


    {NITF_BCS_A, 13, "Synthetic aperture origin point (m) (x component)", 
     "AP_ORIGIN_X" 
    },


    {NITF_BCS_A, 13, "Sythetic aperture origin point (m) (y component)", 
     "AP_ORIGIN_Y" 
    },

    {NITF_BCS_A, 13, "Sythetic aperture origin point (m) (z component)", 
     "AP_ORIGIN_Z" 
    },

    {NITF_BCS_A, 16, "Sythetic aperture direction unit vec. (x component)", 
     "AP_DIR_X" 
    },


    {NITF_BCS_A, 16, "Sythetic aperture direction unit vec. (y component)", 
     "AP_DIR_Y" 
    },

    {NITF_BCS_A, 16, "Sythetic aperture direction unit vec. (z component)", 
     "AP_DIR_Z" 
    },


    {NITF_BCS_A, 12, "Start point of synthetic aperture", "X_AP_START" },
    {NITF_BCS_A, 12, "End point of synthetic aperture", "X_AP_END" },

    {NITF_BCS_N, 4, "Spot stitching row shift", "SS_ROW_SHIFT" },
    {NITF_BCS_N, 4, "Spot stitching column shift", "SS_COL_SHIFT" },

    {NITF_BCS_A, 16, "Semi-major axis (x component)", "U_hat_x" },
    {NITF_BCS_A, 16, "Semi-major axis (y component)", "U_hat_y" },
    {NITF_BCS_A, 16, "Semi-major axis (z component)", "U_hat_z" },

    {NITF_BCS_A, 16, "Semi-minor axis (x component)", "V_hat_x" },
    {NITF_BCS_A, 16, "Semi-minor axis (y component)", "V_hat_y" },
    {NITF_BCS_A, 16, "Semi-minor axis (z component)", "V_hat_z" },

    {NITF_BCS_A, 16, "Normal vector (x component)", "N_hat_x" },
    {NITF_BCS_A, 16, "Normal vector (y component)", "N_hat_y" },
    {NITF_BCS_A, 16, "Normal vector (z component)", "N_hat_z" },
    
    {NITF_BCS_A, 16, "Dummy param, leading edge of patch", "Eta_0" },

    {NITF_BCS_A, 13, "Semi-major axis length", "Sigma_sm" },
    {NITF_BCS_A, 13, "Semi-minor axis length", "Sigma_sn" },

    {NITF_BCS_A, 10, 
     "Offset between small circle and great circle plane (m)",
     "S_off" },

    {NITF_BCS_N, 12, 
     "Offset between small circle and great circle plane (m)",
     "Rn_offset" },

    {NITF_BCS_N, 16, 
     "Radius of curvature of scene center line (m)",
     "R_scl" },

    {NITF_BCS_N, 16, 
     "Radius of curvature of nominal aircraft flight track (m)",
     "R_nav" },

    {NITF_BCS_N, 16, 
     "Cross-track offset to scene center line (m)",
     "R_sc_exact" },
    

    {NITF_BCS_A, 16, 
     "Center of scene center line fit circle, ECEF (m) (x component)",
     "C_sc_x" },
    {NITF_BCS_A, 16, 
     "Center of scene center line fit circle, ECEF (m) (y component)",
     "C_sc_y" },
    {NITF_BCS_A, 16, 
     "Center of scene center line fit circle, ECEF (m) (z component)",
     "C_sc_z" },


    {NITF_BCS_A, 16, 
     "Local radial direction unit vector, leading edge of patch (x component)",
     "K_hat_x" },
    {NITF_BCS_A, 16, 
     "Local radial direction unit vector, leading edge of patch (y component)",
     "K_hat_y" },
    {NITF_BCS_A, 16, 
     "Local radial direction unit vector, leading edge of patch (z component)",
     "K_hat_z" },

    {NITF_BCS_A, 16, 
     "Local along-track unit vector, leading edge of patch (x component)",
     "L_hat_x" },
    {NITF_BCS_A, 16, 
     "Local along-track unit vector, leading edge of patch (y component)",
     "L_hat_y" },
    {NITF_BCS_A, 16, 
     "Local along-track unit vector, leading edge of patch (z component)",
     "L_hat_z" },

    {NITF_BCS_A, 16, 
     "Perp. distance from nav. circle to scene center line point",
     "P_Z" },

    {NITF_BCS_A, 16, 
     "Perp. distance from nav. circle to scene center line point",
     "Theta_c" },

    {NITF_BCS_N, 16, 
     "Tangent cone apex parameter",
     "Alpha_sl" },

    {NITF_BCS_N, 16, 
     "Tangent cone scale parameter",
     "Sigma_tc" },

    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(ASTORA, description)

NITF_CXX_ENDGUARD
