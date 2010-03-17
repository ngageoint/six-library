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
#ifndef __SIX_ANTENNA_H__
#define __SIX_ANTENNA_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
namespace sicd
{
    /*!
     *  \struct ElectricalBoresight
     *  \brief SICD EB Parameter
     *
     *  Electrical boresight steering directions
     *  the DCXPoly is the EB sterring x-axis direction
     *  cosine (DCX) as a function of slow time
     */
    struct ElectricalBoresight
    {
	//! Constructor
        ElectricalBoresight() {}

	//! Destructor
        ~ElectricalBoresight() {}

	//! Clone the object
        ElectricalBoresight* clone() const;

	//! SICD DCXPoly
        Poly1D dcxPoly;

	//! SICD DCYPoly
        Poly1D dcyPoly;
    };

    /*!
     *  \struct HalfPowerBeamwidths
     *  \brief SICD HPBW Parameter
     *
     *  Half-power beamwidths.  For electronically steered
     *  arrays, the EB is steered to DCX = 0, DCY = 0
     */
    struct HalfPowerBeamwidths
    {
	//! Constructor
        HalfPowerBeamwidths();

	//! Destructor
        ~HalfPowerBeamwidths(){}

	//! Clone the object
        HalfPowerBeamwidths* clone() const;
       
	//! SICD DCX
        double dcx;

	//! SICD DCY
        double dcy;
    };

    /*!
     *  \struct GainAndPhasePolys
     *  \brief SICD Array & Elem Parameter
     *
     *  Mainlobe array pattern polynomials.  For
     *  electronically steered arrays, the EB is steered to
     *  DCX = 0, DCY = 0.
     */
    struct GainAndPhasePolys
    {        
        //! No init right now, could do that and set const coef to zero
        GainAndPhasePolys() {}

	//! Destructor
        ~GainAndPhasePolys() {}

        GainAndPhasePolys* clone() const;

        //! One way signal gain (in dB) as a function of DCX and DCY
        //! Gain relative to gain at DCX = 0, DCY = 0.  Const coeff = 0 always
        Poly2D gainPoly;

        //! One way signal phase (in cycles) as a function of DCX and DCY.
        //! Phase relative to phase at DCX = 0, DCY = 0. Const coeff = 0 always
        Poly2D phasePoly;
    };

    /*!
     *  \struct AntennaParameters
     *  \brief SICD Tx/Rcv, etc Antenna parameter
     *
     */
    struct AntennaParameters
    {
	
	//!  Constructor
        AntennaParameters() : 
        electricalBoresight(NULL), halfPowerBeamwidths(NULL), array(NULL),
            element(NULL)
        {
            electricalBoresightFrequencyShift = Init::undefined<BooleanType>();
            mainlobeFrequencyDilation = Init::undefined<BooleanType>();

        }

        //! Deletes any non-NULL children
        ~AntennaParameters();

        //! Deep copy of this, and any initialized pointer children
        AntennaParameters* clone() const;

        //! Aperture X-axis direction in ECF as a function of time
        PolyXYZ xAxisPoly;
        //! Aperture Y-axis direction in ECF as a function of time
        PolyXYZ yAxisPoly;

        //! RF frequency for the HP beamwidths, array and element patterns
        //! and EB steering direction cosines
        double frequencyZero;

        //! Electrical boresight params
        ElectricalBoresight* electricalBoresight;

        //! Half-power beamwidths
        HalfPowerBeamwidths* halfPowerBeamwidths;

        /*!
         *  Mainlobe array pattern polynomials.  For
         *  electronically steered arrays, the EB is steered to
         *  DCX = 0 and DCY = 0
         */
        GainAndPhasePolys* array;

        /*!
         *  Element array pattern polynomials for electronically
         *  steered arrays
         */
        GainAndPhasePolys* element;

        /*!
         *  Gain polynomial (dB) vs. frequency for boresight
         *  at DCX = 0 and DCY = 0.  Frequency ratio input variable
         *  Const coeff = 0 always.
         */
        Poly1D gainBSPoly;

        // These are optional and are unset by default

        /*!
         *  Parameter indictating the EB shifts with frequency for an
         *  electronically steered array.  TS_FALSE = No shift with frequency
         *  TS_TRUE = Shift with frequency per ideal array theory
         */
        BooleanType electricalBoresightFrequencyShift;

        /*!
         *  Parameter indication the mainlobe width chainges with
         *  frequency.  BOOL_FALSE = No change with frequency.  BOOL_TRUE =
         *  Change with frequency per ideal array theory
         */
        BooleanType mainlobeFrequencyDilation;

        
    };


    /*!
     *  \struct Antenna
     *  \brief SICD Antenna parameter
     *
     *  Parameters that describe the antenna(s) during collection.
     *  Parameters describe the antenna orientation, pointing direction
     *  and beamshape during the collection.
     *
     *  Parameters may be provided separably for the transmit (Tx) antenna
     *  and the receieve (Rcv) antenna.  A single set of prarameters may
     *  be provided for a combined two-way pattern (as appropriate)
     */
    struct Antenna
    {
        //! Transmit parameters
        AntennaParameters* tx;

        //! Receieve parameters
        AntennaParameters* rcv;

        //! Two way parameters
        AntennaParameters* twoWay;

	//!  Constructor
        Antenna() : tx(NULL), rcv(NULL), twoWay(NULL) {}

	//! Destructor
        ~Antenna();

        /*!
         *  Make a copy of each antenna parameter if it exists
         */
        Antenna* clone() const;
    };


}
}
#endif
