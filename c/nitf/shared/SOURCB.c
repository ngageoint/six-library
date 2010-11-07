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
    {NITF_BCS_N, 9, "Image Segment Reciprocal Scale", "IS_SCA" },
    {NITF_BCS_A, 10, "Colout Patch ID", "CPATCH" },
    {NITF_BCS_N, 2, "Number of Source Descriptions", "NUM_SOUR" },
    {NITF_LOOP, 0, NULL, "NUM_SOUR"},
    {NITF_BCS_N, 2, "Number of Bounding Polygons", "NUM_BP" },
    {NITF_LOOP, 0, NULL, "NUM_BP"},
    {NITF_BCS_N, 3, "Number of Points in the Bounding Polygon", "NUM_PTS" },
    {NITF_LOOP, 0, NULL, "NUM_PTS"},
    {NITF_BCS_N, 15, "Longitude/Easting", "LON" },
    {NITF_BCS_N, 15, "Latitude/Northing", "LAT" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_A, 10, "Series", "PRT" },
    {NITF_BCS_A, 20, "Source Identification", "URF" },
    {NITF_BCS_A, 7, "Edition", "EDN" },
    {NITF_BCS_A, 20, "Name", "NAM" },
    {NITF_BCS_N, 3, "Type of Significant Date", "CDP" },
    {NITF_BCS_A, 8, "Significant Date", "CDV" },
    {NITF_BCS_A, 8, "Perishable Date", "CDV27" },
    {NITF_BCS_A, 80, "Source Reference Number", "SRN" },
    {NITF_BCS_A, 9, "Reciprocal Scale", "SCA" },
    {NITF_BCS_A, 3, "Unit of Measure for Coverage", "UNISQU" },
    {NITF_BCS_N, 10, "Coverage", "SQU" },
    {NITF_BCS_A, 3, "Unit of Measure for Contour Interval", "UNIPCI" },
    {NITF_BCS_N, 4, "Contour Interval", "PCI" },
    {NITF_BCS_N, 3, "Water Coverage", "WPC" },
    {NITF_BCS_N, 3, "Navigation System Type", "NST" },
    {NITF_BCS_A, 3, "Units of HKE", "UNIHKE" },
    {NITF_BCS_N, 6, "Highest Elevation", "HKE" },
    {NITF_BCS_N, 15, "Longitude/Easting of HKE", "LONHKE" },
    {NITF_BCS_N, 15, "Latitude/Northing of HKE", "LATHKE" },
    {NITF_BCS_A, 1, "Security Classification of Source", "QSS" },
    {NITF_BCS_A, 1, "Downgrading", "QOD" },
    {NITF_BCS_A, 8, "Downgrading Date", "CDV10" },
    {NITF_BCS_A, 80, "Releasibility", "QLE" },
    {NITF_BCS_A, 80, "Copyright Statement", "CPY" },
    {NITF_BCS_N, 2, "Number of Magnetic Information", "NMI" },
    {NITF_LOOP, 0, NULL, "NMI"},
    {NITF_BCS_A, 8, "Date of Magnetic Information", "CDV30" },
    {NITF_BCS_A, 3, "Units for Annual Rate of Change", "UNIRAT" },
    {NITF_BCS_N, 8, "Annual Rate of Change", "RAT" },
    {NITF_BCS_A, 3, "Units of GMA", "UNIGMA" },
    {NITF_BCS_N, 8, "GM Angle", "GMA" },
    {NITF_BCS_N, 15, "Longitude/Easting of GMA Reference Point", "LONGMA" },
    {NITF_BCS_N, 15, "Latitude/Northing of GMA Reference Point", "LATGMA" },
    {NITF_BCS_A, 3, "Units of GCA", "UNIGCA" },
    {NITF_BCS_N, 8, "Grid Convergence Angle", "GCA" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 2, "Number of Legend Images", "NLI" },
    {NITF_LOOP, 0, NULL, "NLI"},
    {NITF_BCS_A, 10, "Legend ID", "BAD" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_A, 80, "Geodetic Datum Name", "DAG" },
    {NITF_BCS_A, 4, "Geodetic Datum Code", "DCD" },
    {NITF_BCS_A, 80, "Ellipsoid Name", "ELL" },
    {NITF_BCS_A, 3, "Ellipsoid Code", "ELC" },
    {NITF_BCS_A, 80, "Vertical Datum Reference", "DVR" },
    {NITF_BCS_A, 4, "Code of Vertical Reference", "VDCDVR" },
    {NITF_BCS_A, 80, "Sounding Datum Name", "SDA" },
    {NITF_BCS_A, 4, "Code for Sounding Datum", "VDCSDA" },
    {NITF_BCS_A, 80, "Projection Name", "PRN" },
    {NITF_BCS_A, 2, "Projection Code", "PCO" },
    {NITF_BCS_N, 1, "Number of Projection Parameters", "NUM_PRJ" },
    {NITF_LOOP, 0, NULL, "NUM_PRJ"},
    {NITF_BCS_N, 15, "Projection Parameter", "PRJ" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_BCS_N, 15, "Projection False X (Easting) Origin", "XOR" },
    {NITF_BCS_N, 15, "Projection False Y (Northing) Origin", "YOR" },
    {NITF_BCS_A, 3, "Grid Code", "GRD" },
    {NITF_BCS_A, 80, "Grid Description", "GRN" },
    {NITF_BCS_N, 4, "Grid Zone number", "ZNA" },
    {NITF_BCS_N, 2, "Number of Insets", "NIN" },
    {NITF_LOOP, 0, NULL, "NIN"},
    {NITF_BCS_A, 10, "Inset Identification", "INT" },
    {NITF_BCS_N, 9, "Reciprocal Scale of Inset", "INS_SCA" },
    {NITF_BCS_N, 15, "Absolute longitude of lower left corner", "NTL" },
    {NITF_BCS_N, 15, "Absolute latitude of lower left corner", "TTL" },
    {NITF_BCS_N, 15, "Absolute longitude of upper left corner", "NVL" },
    {NITF_BCS_N, 15, "Absolute latitude of upper left corner", "TVL" },
    {NITF_BCS_N, 15, "Absolute longitude of upper right corner", "NTR" },
    {NITF_BCS_N, 15, "Absolute latitude of upper right corner", "TTR" },
    {NITF_BCS_N, 15, "Absolute longitude of lower right corner", "NVR" },
    {NITF_BCS_N, 15, "Absolute latitude of lower right corner", "TVR" },
    {NITF_BCS_N, 15, "Relative longitude of lower left corner", "NRL" },
    {NITF_BCS_N, 15, "Relative latitude of lower left corner", "TRL" },
    {NITF_BCS_N, 15, "Relative longitude of upper left corner", "NSL" },
    {NITF_BCS_N, 15, "Relative latitude of upper left corner", "TSL" },
    {NITF_BCS_N, 15, "Relative longitude of upper right corner", "NRR" },
    {NITF_BCS_N, 15, "Relative latitude of upper right corner", "TRR" },
    {NITF_BCS_N, 15, "Relative longitude of lower right corner", "NSR" },
    {NITF_BCS_N, 15, "Relative latitude of lower right corner", "TSR" },
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_ENDLOOP, 0, NULL, NULL},
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(SOURCB, description)

NITF_CXX_ENDGUARD
