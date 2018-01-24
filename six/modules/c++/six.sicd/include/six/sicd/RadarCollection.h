/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <cmath>
#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"
#include <mem/ScopedCloneablePtr.h>
#include <logging/Logger.h>

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

    //! Equality operator
    bool operator==(const TxStep& rhs)
    {
        return waveformIndex == rhs.waveformIndex && txPolarization == txPolarization;
    }

    bool operator!=(const TxStep& rhs)
    {
        return !(*this == rhs);
    }
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

    //! Equality operator
    bool operator==(const WaveformParameters& rhs) const;
    bool operator!=(const WaveformParameters& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDerivedFields();
    bool validate(int refFrequencyIndex, logging::Logger& log) const;
private:
    static const double WF_TOL;
    static const double WGT_TOL;
    static const char WF_INCONSISTENT_STR[];
};

/*!
 *  \struct ChannelParameters
 *  \brief SICD 'ChanParameters' parameter
 *
 *  Parameters for a data channel.
 */
struct ChannelParameters
{
    //!  Constructor
    ChannelParameters();

    //!  Make a copy
    ChannelParameters* clone() const;

    /*!
     *  Indicates the combined transmit and receive polarization ("TX:RCV")
     *  for the channel.
     *  Optional for 0.4, required for 1.0.
     */
    DualPolarizationType txRcvPolarization;

    /*!
     *  (Optional) RcvAPCIndex is the index of the receive
     *  aperture phase center (APC).  Only included if the Rcv
     *  APC polynomials are included
     */
    int rcvAPCIndex;

    //! Equality operator
    bool operator==(const ChannelParameters& rhs) const
    {
        return (txRcvPolarization == rhs.txRcvPolarization &&
            rcvAPCIndex == rhs.rcvAPCIndex);
    }

    bool operator!=(const ChannelParameters& rhs) const
    {
        return !(*this == rhs);
    }
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

    //!  The unit vector in the geo-referenced plane's direction
    Vector3 unitVector;

    //!  The spacing in lines/samples (SICD LineSpacing/SampleSpacing)
    double spacing;

    //!  The number of elements in this direction (SICD NumLines/NumSamples)
    size_t elements;

    //!  The first element in this direction (SICD FirstLine/FirstSample)
    size_t first;

    //! Returns the extent in meters
    double getExtentInMeters() const
    {
        // meters/pixel * numPixels
        return (spacing * elements);
    }

    bool operator==(const AreaDirectionParameters& other) const
    {
        return unitVector == other.unitVector &&
               spacing == other.spacing &&
               elements == other.elements &&
               first == other.first;
    }

    bool operator!=(const AreaDirectionParameters& other) const
    {
        return !((*this) == other);
    }
};

std::ostream& operator<< (std::ostream& os, const AreaDirectionParameters& d);

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

    //! The number of lines in the segment
    size_t getNumLines() const
    {
        // Rotating can make the start/end in reverse order,
        // so need the absolute value
        return std::abs(endLine - startLine) + 1;
    }

    //! The number of samples in the segment
    size_t getNumSamples() const
    {
        return std::abs(endSample - startSample) + 1;
    }

    /*!
     *  SICD StartLine parameter.  Defines start line in collection plane
     *  corresponding to the full image.  This is inclusive.  It will normally
     *  be 0-based or 1-based but does not necessarily have to be.
     */
    int startLine;

    /*!
     *  SICD StartSample parameter.  Defines start sample in collection plane
     *  corresponding to the full image.  This is inclusive.  It will normally
     *  be 0-based or 1-based but does not necessarily have to be.
     */
    int startSample;

    /*!
     *  SICD EndLine parameter.  Defines end line in collection plane
     *  corresponding to the full image.  This is inclusive.  It will normally
     *  be 0-based or 1-based but does not necessarily have to be.
     */
    int endLine;

    /*!
     *  SICD EndSample parameter.  Defines end sample in collection plane
     *  corresponding to the full image.  This is inclusive.  It will normally
     *  be 0-based or 1-based but does not necessarily have to be.
     */
    int endSample;

    /*!
     *  SICD Identifier parameter for data boundary defined by Segment
     */
    std::string identifier;

    //! Equality operator
    bool operator==(const Segment& rhs) const;
    bool operator!=(const Segment& rhs) const
    {
        return !(*this == rhs);
    }

    /*!
     * Rotate the segment counter clockwise
     * \param numColumns How many columns are in the plane
     */
    void rotateCCW(size_t numColumns);

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
     * Provides the reference point as pixel-centered 0-based and taking
     * the FirstLine/FirstSample offsets into account
     */
    types::RowCol<double> getAdjustedReferencePoint() const;

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

    //! Equality operator
    bool operator==(const AreaPlane& rhs) const
    {
        return (referencePoint == rhs.referencePoint && xDirection == rhs.xDirection &&
            yDirection == rhs.yDirection && segmentList == rhs.segmentList &&
            orientation == rhs.orientation);
    }
    bool operator!=(const AreaPlane& rhs) const
    {
        return !(*this == rhs);
    }

    /*!
     * Find the first segment with the given identifier
     * Throws if no segments match
     * \param segmentId The Segment identifier
     * \return Reference to Segment
     */
    const Segment& getSegment(const std::string& segmentId) const;

    /*!
     * Rotate the plane counterclockwise, updating the fields to match
     */
    void rotateCCW();

    /*!
     * Rotate the plane until it is shadows down.
     * If the plane's orientation type is ARBITRARY or NOT_SET (or DOWN),
     * no rotation will occur
     */
    void rotateToShadowsDown();
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

    //! Equality operator
    bool operator==(const Area& rhs) const
    {
        return acpCorners == rhs.acpCorners && plane == rhs.plane;
    }
    bool operator!=(const Area& rhs) const
    {
        return !(*this == rhs);
    }
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
        txFrequencyMin(Init::undefined<double>()),
        txFrequencyMax(Init::undefined<double>()),
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

    //! Polarization type (optional in 0.4, required in 1.x)
    PolarizationSequenceType txPolarization;

    //! Optional Polarization HV ange (rad) vs time.  achieved H&V channel
    //! orientation relative to true H&V, t = 0 at coll start
    //! Optional in 0.4, removed in 1.0
    Poly1D polarizationHVAnglePoly;

    //!  Optional, indicates the transmit signal steps through
    //!  a repeating sequence of waveforms and/or polarizations, one step
    //!  per IPP.
    std::vector<mem::ScopedCloneablePtr<TxStep> > txSequence;

    //!  (Optional) waveform parameters
    std::vector<mem::ScopedCloneablePtr<WaveformParameters> > waveform;

    //!  (Optional) receive channels
    std::vector<mem::ScopedCloneablePtr<ChannelParameters> > rcvChannels;

    //!  (Optional) area parameter
    mem::ScopedCloneablePtr<Area> area;

    //!  (Optional) additional parameters
    ParameterCollection parameters;

    //! Equality operator
    bool operator==(const RadarCollection& rhs) const;
    bool operator!=(const RadarCollection& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDerivedFields();
    bool validate(logging::Logger& log) const;
private:
    double waveformMin() const;
    double waveformMax() const;
    static const double WF_TOL;
    static const char WF_INCONSISTENT_STR[];
};

}
}
#endif

