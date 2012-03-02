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
#ifndef __SIX_RADAR_COLLECTION_H__
#define __SIX_RADAR_COLLECTION_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include <mem/ScopedCloneablePtr.h>

namespace six
{
namespace sicd
{
/*!
 *  \struct TxStep
 *  \brief Transmit sequence step index.  Starts at one
 *
 *  Represents a transmit sequence step
 */
struct TxStep
{
    //!  Constructor
    TxStep();
    
    //!  Destructor
    ~TxStep() {}
  
    //!  Clone this object
    TxStep* clone() const;

    //! Optional waveform index for this step
    int waveformIndex;

    //! Transmit signal polarization for this step
    PolarizationType txPolarization;
};

/*!
 *  \struct WaveformParameters
 *  \brief SICD WFParameter parameter
 *
 *  Transmit and receive demodulation waveform parameters.  Waveforms are
 *  indexed starting at 1 by the index, but this is stored in a vector
 *  and starts at zero as a result.  The serializer hides this.
 */
struct WaveformParameters
{
    //!  Constructor
    WaveformParameters();

    //!  Destructor
    ~WaveformParameters() {}

    //!  Clone
    WaveformParameters* clone() const;

    //! Transmit pulse length
    double txPulseLength;

    //! Transmit RF bandiwth of the transmit pulse
    double txRFBandwidth;

    //! Tramsit start freq for linear FM waveform
    double txFrequencyStart;

    //! Transmit FM rate for linear FM waveform
    double txFMRate;

    //! Receive demodulation used when linear FM waveform is
    //! used on transmit
    DemodType rcvDemodType;

    //! Receive window duration
    double rcvWindowLength;

    //! Analog to digital converter sampling rate
    double adcSampleRate;

    //! Receive IF bandidth
    double rcvIFBandwidth;

    //! Frequency demodulation start
    double rcvFrequencyStart;

    //! Receive FM rate.  Set to 0 for demod = CHIRP
    double rcvFMRate;
};

/*!
 *  \struct ChannelParameters
 *  \brief SICD 'ChanParameters' parameter
 *
 *  Parameters for a data channel.  All parameters are optional
 */
struct ChannelParameters
{

    //!  Constructor
    ChannelParameters();

    //!  Destructor
    ~ChannelParameters() {}

    //!  Make a copy
    ChannelParameters* clone() const;

    /*!
     *  (Optional) RcvAPCIndex is the index of the receive
     *  aperture phase center (APC).  Only included if the Rcv
     *  APC polynomials are included
     */
    int rcvAPCIndex;

    /*!
     *  Indicates the combined transmit and receive polarization ("TX:RCV")
     *  for the channel
     */
    DualPolarizationType txRcvPolarization;
};

/*!
 *  \struct AreaDirectionParameters
 *  \brief SICD XDir, YDir parameters
 *
 *  This is an internal grouping of the XDir and YDir parameters
 *  associated with a SICD.  It includes  the unit vector, the sample
 *  spacing, and the first element
 */
struct AreaDirectionParameters
{
    //!  Constructor
    AreaDirectionParameters();
    
    //!  Clone (copy) params
    AreaDirectionParameters* clone() const;

    //!  Dont have to delete anything here
    ~AreaDirectionParameters() {}

    //!  The unit vector in the geo-referenced plane's direction
    Vector3 unitVector;

    //!  The spacing in lines/samples (SICD LineSpacing/SampleSpacing)
    double spacing;

    //!  The number of elements in this direction (SICD NumLines/NumSamples)
    unsigned long elements;

    //!  The first element in this direction (SICD FirstLine/FirstSample)
    unsigned long first;
};

/*!
 *  \struct Segment
 *  \brief SICD Segment parameter
 *
 *  Defines the data boundaries of the segment.  Segments are indexed
 *  x = 1,2... number of segments in the radar collection plane.
 *  The segment index is handled automatically on serialization.
 *  On SICD read, this segment assumes that the 'index' is redundant,
 *  since it is specified in-order according to the SICD D&E. Therefore
 *  the index value on XML read is completely ignored by this library
 *
 *  Contains also the start line/sample and end/line sample for each segment
 */
struct Segment
{
    //!  Construct
    Segment();

    //!  Empty destructor
    ~Segment() {}

    //!  Copy
    Segment* clone() const;

    /*!
     *  SICD StartLine parameter.  Defines start line in collection plane
     *  corresponding to the full image.
     */
    int startLine;

    /*!
     *  SICD StartSample parameter.  Defines start sample in collection plane
     *  corresponding to the full image
     */
    int startSample;

    /*!
     *  SICD EndLine parameter.  Defines end line in collection plane
     *  corresponding to the full image.
     */
    int endLine;
    
    /*!
     *  SICD EndSample parameter.  Defines end sample in collection plane
     *  corresponding to the full image
     */    
    int endSample;

    /*!
     *  SICD Identifier parameter for data boundary defined by Segment
     */
    std::string identifier;

};

/*!
 *  \struct AreaPlane
 *  \brief SICD Plane parameter
 *
 *  This object contains parameters describing a rectangular
 *  area in a geo-referenced display plane
 */
struct AreaPlane
{
    /*!
     *  Constructor
     *  Automatically allocates required xDirection and yDirection
     *
     */
    AreaPlane();
    
    /*!
     *  Make a deep copy of xDirection, yDirection, and any existing
     *  segments
     */
    AreaPlane* clone() const;

    /*!
     *  SICD RefPt. Represents the reference point
     */
    ReferencePoint referencePoint;

    /*!
     *  SICD XDir.  Represents parameters in the X direction of the
     *  geo-referenced display plane.  X direction is also the increasing
     *  line direction.
     *
     */
    mem::ScopedCloneablePtr<AreaDirectionParameters> xDirection;

    /*!
     *  SICD YDir.  Represents parameters in the Y direction of the
     *  geo-referenced display plane.  Y direction is also the increasing
     *  samples direction.
     */
    mem::ScopedCloneablePtr<AreaDirectionParameters> yDirection;

    /*!
     *  (Optional) list of segments corresponding to the RadarCollection
     *  AreaPlane.
     */
    std::vector<mem::ScopedCloneablePtr<Segment> > segmentList;

     /*!
      * Orientation type describing the shadow intent of the geo-reference
      * display plane defined above.
      */
    OrientationType orientation;
};

/*!
 *  \struct Area
 *  \brief SICD Area parameters
 *
 *  (Optional) Parameters describing the image area covered by the collection.
 */
struct Area
{

    /*!
     *  Constructor.  Does not create plane, and leaves the corners undefined,
     *  since both parameters are optional.  It is up to the user to allocate
     *  this
     */
    Area();

    //!  Clone the object, including the plane if non-NULL
    Area* clone() const;

    /*!
     *  SICD ACP parameter.  LLH representation of the corner point
     *  parameters.
     */
    LatLonAltCorners acpCorners;

    /*!
     *  (Optional) SICD Plane parameter, See AreaPlane.
     *  
     */
    mem::ScopedCloneablePtr<AreaPlane> plane;
};

/*!
 *  \struct RadarCollection
 *  \brief SICD RadarCollection block
 *
 *  Describes the radar collection information.  The SICD RefFreqIndex,
 *  called here refFrequencyIndex for API consistency is not initialized,
 *  since it is optional.  If required for profile, it must be manually 
 *  populated by the application developer.
 *
 */
struct RadarCollection
{

    /*!
     *  The SICD Res parameter (called resolution here) is optional, and
     *  therefore is set to NULL, as is Area
     */
    RadarCollection() :
        refFrequencyIndex(Init::undefined<int>()),
        polarizationHVAnglePoly(Init::undefined<Poly1D>())
    {
    }

    //!  Clone.  Makes a deep copy of resolution if non-NULL
    RadarCollection* clone() const;

    //! Indicates the RF freq values expressed as offsets from a ref freq
    int refFrequencyIndex;

    //! Minimum transmitted RF frequency
    double txFrequencyMin;
    //! Maximum transmitted RF frequency
    double txFrequencyMax;

    //! Optional polarization type
    PolarizationType txPolarization;

    //! Optional Polarization HV ange (rad) vs time.  achieved H&V channel
    //! orientation relative to true H&V, t = 0 at coll start
    Poly1D polarizationHVAnglePoly;

    //!  Optional, indicates the transmit signal teps trhough
    //!  a repeating sequence of waveforms and/or polarizations, one step
    //!  per IPP.
    std::vector<mem::ScopedCloneablePtr<TxStep> > txSequence;

    //!  (Optional) waveform pararmeters
    std::vector<mem::ScopedCloneablePtr<WaveformParameters> > waveform;

    //!  (Optional) receive channels
    std::vector<mem::ScopedCloneablePtr<ChannelParameters> > rcvChannels;

    //!  (Optional) area parameter
    mem::ScopedCloneablePtr<Area> area;

    //!  (Optional) additional parameters
    std::vector<Parameter> parameters;
};

}
}
#endif
