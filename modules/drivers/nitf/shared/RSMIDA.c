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
    {NITF_BCS_A, 40, "Image Sequence Identifier", "ISID" },
    {NITF_BCS_A, 40, "Sensor Identifier", "SID" },
    {NITF_BCS_A, 40, "Sensor Type Identifier", "STID" },
    {NITF_BCS_N, 4, "Year of Image Acquisition", "YEAR" },
    {NITF_BCS_N, 2, "Month of Image Acquisition", "MONTH" },
    {NITF_BCS_N, 2, "Day of Image Acquisition", "DAY" },
    {NITF_BCS_N, 2, "Hour of Image Acquisition", "HOUR" },
    {NITF_BCS_N, 2, "Minute of Image Acquisition", "MINUTE" },
    {NITF_BCS_N, 9, "Second of Image Acquisition", "SECOND" },
    {NITF_BCS_A, 8, "Number of Rows Acquired Simultaneously", "NRG" },
    {NITF_BCS_A, 8, "Number of Columns Acquired Simultaneously", "NCG" },
    {NITF_BCS_A, 21, "Time Between Adjacent Row Groups", "TRG" },
    {NITF_BCS_A, 21, "Time Between Adjacent Column Groups", "TCG" },

    {NITF_BCS_A, 1, "Ground Domain Form", "GRNDD" },
    {NITF_BCS_A, 21, "Regular Coordinate Origin (XUOR)", "XUOR" },
    {NITF_BCS_A, 21, "Regular Coordinate Origin (YUOR)", "YUOR" },
    {NITF_BCS_A, 21, "Regular Coordinate Origin (ZUOR)", "ZUOR" },

    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (XUXR)", "XUXR" },
    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (XUYR)", "XUYR" },
    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (XUZR)", "XUZR" },

    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (YUXR)", "YUXR" },
    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (YUYR)", "YUYR" },
    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (YUZR)", "YUZR" },

    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (ZUXR)", "ZUXR" },
    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (ZUYR)", "ZUYR" },
    {NITF_BCS_A, 21, "Rectangular Coord Unit Vector (ZUZR)", "ZUZR" },

    {NITF_BCS_A, 21, "Vertex 1 - X coord of the RSM ground domain", "V1X" },
    {NITF_BCS_A, 21, "Vertex 1 - Y coord of the RSM ground domain", "V1Y" },
    {NITF_BCS_A, 21, "Vertex 1 - Z coord of the RSM ground domain", "V1Z" },

    {NITF_BCS_A, 21, "Vertex 2 - X coord of the RSM ground domain", "V2X" },
    {NITF_BCS_A, 21, "Vertex 2 - Y coord of the RSM ground domain", "V2Y" },
    {NITF_BCS_A, 21, "Vertex 2 - Z coord of the RSM ground domain", "V2Z" },

    {NITF_BCS_A, 21, "Vertex 3 - X coord of the RSM ground domain", "V3X" },
    {NITF_BCS_A, 21, "Vertex 3 - Y coord of the RSM ground domain", "V3Y" },
    {NITF_BCS_A, 21, "Vertex 3 - Z coord of the RSM ground domain", "V3Z" },

    {NITF_BCS_A, 21, "Vertex 4 - X coord of the RSM ground domain", "V4X" },
    {NITF_BCS_A, 21, "Vertex 4 - Y coord of the RSM ground domain", "V4Y" },
    {NITF_BCS_A, 21, "Vertex 4 - Z coord of the RSM ground domain", "V4Z" },

    {NITF_BCS_A, 21, "Vertex 5 - X coord of the RSM ground domain", "V5X" },
    {NITF_BCS_A, 21, "Vertex 5 - Y coord of the RSM ground domain", "V5Y" },
    {NITF_BCS_A, 21, "Vertex 5 - Z coord of the RSM ground domain", "V5Z" },


    {NITF_BCS_A, 21, "Vertex 6 - X coord of the RSM ground domain", "V6X" },
    {NITF_BCS_A, 21, "Vertex 6 - Y coord of the RSM ground domain", "V6Y" },
    {NITF_BCS_A, 21, "Vertex 6 - Z coord of the RSM ground domain", "V6Z" },

    {NITF_BCS_A, 21, "Vertex 7 - X coord of the RSM ground domain", "V7X" },
    {NITF_BCS_A, 21, "Vertex 7 - Y coord of the RSM ground domain", "V7Y" },
    {NITF_BCS_A, 21, "Vertex 7 - Z coord of the RSM ground domain", "V7Z" },

    {NITF_BCS_A, 21, "Vertex 8 - X coord of the RSM ground domain", "V8X" },
    {NITF_BCS_A, 21, "Vertex 8 - Y coord of the RSM ground domain", "V8Y" },
    {NITF_BCS_A, 21, "Vertex 8 - Z coord of the RSM ground domain", "V8Z" },

    {NITF_BCS_A, 21, "Ground Reference Point X", "GRPX" },
    {NITF_BCS_A, 21, "Ground Reference Point Y", "GRPY" },
    {NITF_BCS_A, 21, "Ground Reference Point Z", "GRPZ" },

    {NITF_BCS_A, 8, "Number of Rows in Full Image", "FULLR" },
    {NITF_BCS_A, 8, "Number of Cols in Full Image", "FULLC" },

    {NITF_BCS_A, 8, "Minimum Row", "MINR" },
    {NITF_BCS_A, 8, "Maximum Row", "MAXR" },

    {NITF_BCS_A, 8, "Minimum Col", "MINC" },
    {NITF_BCS_A, 8, "Maximum Col", "MAXC" },

    {NITF_BCS_A, 21, "Illum Elevation Angle Const Coeff", "IE0" },
    {NITF_BCS_A, 21, "Illum Elevation Angle Coeff Per Row", "IER" },
    {NITF_BCS_A, 21, "Illum Elevation Angle Coeff Per Col", "IEC" },

    {NITF_BCS_A, 21, "Illum Elevation Angle Coeff Per Row^2", "IERR" },
    {NITF_BCS_A, 21, "Illum Elevation Angle Coeff Per Row-Col", "IERC" },
    {NITF_BCS_A, 21, "Illum Elevation Angle Coeff Per Col^2", "IERC" },

    {NITF_BCS_A, 21, "Illum Azimuth Angle Const", "IE0" },
    {NITF_BCS_A, 21, "Illum Azimuth Angle Coeff Per Row", "IAR" },
    {NITF_BCS_A, 21, "Illum Azimuth Angle Coeff Per Col", "IAC" },
    
    {NITF_BCS_A, 21, "Illum Azimuth Angle Coeff Per Row^2", "IARR" },
    {NITF_BCS_A, 21, "Illum Azimuth Angle Coeff Per Row-Col", "IARC" },
    {NITF_BCS_A, 21, "Illum Azimuth Angle Coeff Per Col^2", "IARC" },

    {NITF_BCS_A, 21, "Sensor x-position", "SPX" },
    {NITF_BCS_A, 21, "Sensor x-velocity", "SVX" },
    {NITF_BCS_A, 21, "Sensor x-acceleration", "SAX" },

    {NITF_BCS_A, 21, "Sensor y-position", "SPY" },
    {NITF_BCS_A, 21, "Sensor y-velocity", "SVY" },
    {NITF_BCS_A, 21, "Sensor y-acceleration", "SAY" },

    {NITF_BCS_A, 21, "Sensor z-position", "SPZ" },
    {NITF_BCS_A, 21, "Sensor z-velocity", "SVZ" },
    {NITF_BCS_A, 21, "Sensor z-acceleration", "SAZ" },
    {NITF_END, 0, NULL, NULL}    
};

NITF_DECLARE_SINGLE_PLUGIN(RSMIDA, description)

NITF_CXX_ENDGUARD
