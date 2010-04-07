/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_PFA_H__
#define __SIX_PFA_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
namespace sicd
{
/*!
 *  \struct SlowTimeDeskew
 *  \brief SICD STDeskew
 *
 *  Parameters to describe image domain slow time
 *  deskew processing.  Name changed for API consistency
 */
struct SlowTimeDeskew
{
    //!  Constructor
    SlowTimeDeskew()
    {
        applied = Init::undefined<BooleanType>();
    }

    //!  Destructor
    ~SlowTimeDeskew()
    {
    }

    //!  Make a copy
    SlowTimeDeskew* clone() const
    {
        return new SlowTimeDeskew(*this);
    }

    //! Was slow time deskew phase function applied?
    BooleanType applied;

    //! Slow time deskew phase function to perform the slow-time/Kaz
    //!  shift.  2D poly function of image range coord and az coord
    Poly2D slowTimeDeskewPhasePoly;
};

/*!
 *  \struct Compensation
 *  \brief SICD Comp
 *
 *  Parameters to describe types of PFA specific compensations
 *  that may have been applied.  Name changed for API consistency
 */
struct Compensation
{
    //!  Constructor
    Compensation()
    {
        applied = Init::undefined<BooleanType>();
    }

    //!  Destructor
    ~Compensation()
    {
    }

    //! Text describing the type of compensation applied
    std::string type;
    //! Parameter indicating if compenstation has been applied
    BooleanType applied;
    //! Free format field to pass forward compensation info
    std::vector<Parameter> parameters;

};

/*!
 *  \struct PFA
 *  \brief SICD Polar format algorithm block
 *
 *  Parameters used when the image is formed using the polar format
 *  algorithm (imageFormationAlgorithm = PFA).
 *
 *  \note This is currently limited to a one item choice in SICD,
 *  which would make it hard to do anything else
 */
struct PFA
{

    //!  Constructor, nothing defined
    PFA() :
        slowTimeDeskew(NULL)
    {
        focusPlaneNormal = Init::undefined<Vector3>();
        imagePlaneNormal = Init::undefined<Vector3>();
        polarAngleRefTime = Init::undefined<double>();
        krg1 = Init::undefined<double>();
        krg2 = Init::undefined<double>();
        kaz1 = Init::undefined<double>();
        kaz2 = Init::undefined<double>();
    }

    //!  Destructor
    ~PFA();

    //!  Copy this object
    PFA* clone() const;

    /*!
     *  SICD FPN parameter.
     *  Focus plane normal (ECEF), pointing away from center of earth
     */
    Vector3 focusPlaneNormal;

    /*!
     *  SICD IPN parameter.
     *  Image formation plane normal pointing away from center of earth
     */
    Vector3 imagePlaneNormal;

    /*!
     *  SICD PolarAngRefTime (name change for API consistency)
     *  Polar image formation ref. time.  Polar angle = 0 at the ref time
     *  Measured relative to collect start.  Ref time is typically set
     *  to SCP COA time but may be different
     */
    double polarAngleRefTime;

    /*! 
     *  SICD PolarAngPoly (name change for API consistency)
     *  Poly that yields polar angle in radians as a function of slow time
     *  IOW, Time (sec) -> Polar angle (radians)
     */
    Poly1D polarAnglePoly;

    /*!
     *  SICD SpatialFreqSFPoly parametr (name change for API consistency)
     *  Poly that yields the spatial frequency scale factor as a function
     *  of Polar angle
     *  Polar angle (radians) -> KSF
     */
    Poly1D spatialFrequencyScaleFactorPoly;

    //! Minimum range spatial freq (Krg) output from polar->rect resampling
    double krg1;

    //! Maximum range spatial freq (Krg) output from polar->rect resampling
    double krg2;

    //! Minimum az spatial freq output from polar->rect resampling
    double kaz1;

    //! Minimum az spatial freq output from polar->rect resampling
    double kaz2;

    //! SICD STDeskew parameter
    SlowTimeDeskew* slowTimeDeskew;

    //! SICD Comp parameter
    std::vector<Compensation*> compensations;

};

}
}
#endif
