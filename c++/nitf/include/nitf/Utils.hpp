/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_UTILS_HPP__
#define __NITF_UTILS_HPP__

#include "nitf/Utils.h"
#include "nitf/System.hpp"
#include <string>

namespace nitf
{
    /*!
     *  This struct externalizes all of the utilities from
     *  Utils.h that are NITFAPI.  However, some of the functions
     *  in the C NITFAPI are trivial in C++ using std::string
     *  so we dont include those at all
     *
     *  Since its a 1:1 mapping, we'll just inline everything
     *
     */
    struct Utils
    {
        static bool isNumeric(std::string str);

        static bool isAlpha(std::string str);

        static void decimalToGeographic(double decimal, 
                                        int* degrees, 
                                        int* minutes,
                                        double* seconds);
        
        static double geographicToDecimal(int degrees, 
                                          int minutes, 
                                          double seconds);
        
        /*
        // Not sure whether we should actually publicize these?
        static void parseGeographicString(std::string dms,
                                          int *degrees,
                                          int *minutes,
                                          int *seconds) throw(nitf::Exception);
        
        static parseDecimalString(std::string d, double* decimal)
            throw (nitf::Exception);
        */
        static char cornersTypeAsCoordRep(nitf::CornersType type);
        
        
    };

}

#endif
