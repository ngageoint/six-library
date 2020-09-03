/*
 * =========================================================================
 * This file is part of cphd-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * cphd-python is free software; you can redistribute it and/or modify
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
 */

%module(package="pysix") cphd

%feature("autodoc", "1");
%feature("flatnested");

%include <std_string.i>
%include <std_vector.i>

%import "sys.i"
%import "types.i"
%import "mem.i"
%import "six_sicd.i"
%import "six.i"
%import "math_poly.i"
%import "math_linear.i"
%import "scene.i"

%{
#include "import/types.h"
#include "import/cphd.h"
#include "import/six.h"
#include "import/six/sicd.h"
#include "import/sys.h"
#include <numpyutils/numpyutils.h>

using six::Vector3;
%}

%ignore cphd::CPHDXMLControl::toXML(const Metadata& metadata);
%ignore cphd::CPHDXMLControl::toXML(
    const Metadata& metadata,
    const std::vector<std::string>& schemaPaths);

%ignore cphd::CPHDXMLControl::fromXML(const xml::lite::Document* doc);
%ignore cphd::CPHDXMLControl::fromXML(
    const xml::lite::Document* doc,
    const std::vector<std::string>& schemaPaths);

%ignore cphd::CPHDXMLControl::fromXML(const std::string& xmlString);
%ignore cphd::CPHDXMLControl::fromXML(
    const std::string& xmlString,
    const std::vector<std::string>& schemaPaths);

// Nested class renames
%rename(CphdAntenna) cphd::Antenna;
%rename(DataChannel) cphd::Data::Channel;
%rename(DataSupportArray) cphd::Data::SupportArray;
%rename(ChannelTxRcv) cphd::ChannelParameter::TxRcv;
%rename(ChannelAntenna) cphd::ChannelParameter::Antenna;
%rename(TOALFMEclipse) cphd::TOAExtended::LFMEclipse;
%rename(ErrorMonostatic) cphd::ErrorParameters::Monostatic;
%rename(ErrorMonostaticRadarSensor) cphd::ErrorParameters::Monostatic::RadarSensor;
%rename(ErrorBistatic) cphd::ErrorParameters::Bistatic;
%rename(ErrorBistaticRadarSensor) cphd::ErrorParameters::Bistatic::RadarSensor;

%include "cphd/Types.h"
%include "cphd/Enums.h"
%include "cphd/MetadataBase.h"
%include "cphd/FileHeader.h"
%include "cphd/Global.h"
%include "cphd/SceneCoordinates.h"
%include "cphd/ReferenceGeometry.h"
%include "cphd/SupportArray.h"
%include "cphd/ErrorParameters.h"
%include "cphd/ProductInfo.h"
%include "cphd/SupportBlock.h"
%include "cphd/Channel.h"
%include "cphd/Data.h"
%include "cphd/Dwell.h"
%include "cphd/TxRcv.h"
%include "cphd/PVP.h"
%include "cphd/Antenna.h"
%include "cphd/Metadata.h"
%include "cphd/PVPBlock.h"
%include "cphd/CPHDXMLControl.h"
%include "cphd/Wideband.h"
%include "cphd/CPHDReader.h"
%include "cphd/CPHDWriter.h"

%ignore CPHDWriter::writeCPHDData;
%ignore CPHDWriter::close;
%ignore CPHDWriter::writeMetadata;
%ignore CPHDWriter::write;

%extend cphd::FileHeader
{
    std::string __str__()
    {
        std::ostringstream out;
        out << *$self;
        return out.str();
    }
}

%extend cphd::Metadata
{
    std::string __str__()
    {
        std::ostringstream out;
        out << *$self;
        return out.str();
    }
}

%extend cphd::PVPBlock
{
    std::string __str__()
    {
        std::ostringstream out;
        out << *$self;
        return out.str();
    }
}

%extend cphd::PVPBlock
{
    void getPVPdata(size_t channel, size_t data)
    {
        void* buffer = reinterpret_cast<void*>(data);
        $self->getPVPdata(channel, buffer);
    }
}

%extend cphd::PVPBlock
{
%pythoncode
%{
    # List of field names and get[param]/has[param] methods for each PVP parameter in a CPHD PVP
    # object.  We could generate these programmatically, but this is maybe more readable(?)
    PVP_PARAM_GETTERS = {
        'txTime': 'getTxTime',
        'txPos': 'getTxPos',
        'txVel': 'getTxVel',
        'rcvTime': 'getRcvTime',
        'rcvPos': 'getRcvPos',
        'rcvVel': 'getRcvVel',
        'srpPos': 'getSRPPos',
        'aFDOP': 'getaFDOP',
        'aFRR1': 'getaFRR1',
        'aFRR2': 'getaFRR2',
        'fx1': 'getFx1',
        'fx2': 'getFx2',
        'toa1': 'getTOA1',
        'toa2': 'getTOA2',
        'tdTropoSRP': 'getTdTropoSRP',
        'sc0': 'getSC0',
        'scss': 'getSCSS'
    }
    OPTIONAL_PVP_PARAMS = {
        'ampSF': ('hasAmpSF', 'getAmpSF'),
        'fxN1': ('hasFxN1', 'getFxN1'),
        'fxN2': ('hasFxN2', 'getFxN2'),
        'signal': ('hasSignal', 'getSignal'),
        'tdIonoSRP': ('hasTDIonoSRP', 'getTdIonoSRP'),
        'toaE1': ('hasToaE1', 'getTOAE1'),
        'toaE2': ('hasToaE2', 'getTOAE2')
    }

    import numpy  # 'as np' doesn't work unless the import is within each function

    @staticmethod
    def _pvpFormatToNPdtype(pvpFormatStr):
        """
        \brief  Maps valid PVP format strings (CPHD Spec Table 10-2) to NumPy dtypes
                Currently doesn't support multiple parameters with different types,
                e.g. 'X=U2,Y=F4'

        \param  pvpFormatStr (str)
                CPHD PVP format string, e.g. 'U1' or 'CI2'. See CPHD Spec Table 10-2

        \return NumPy dtype corresponding to pvpFormatStr
        """

        if '=' in pvpFormatStr and ';' in pvpFormatStr:
            # Multiple parameters, assert that they are all the same type
            paramTypes = [param[param.index('=')+1:] for param in pvpFormatStr.split(';') if param]

            # TODO support multiple different parameter types ('A=U2;B=I2')
            if not all(paramType == paramTypes[0] for paramType in paramTypes[1:]):
                raise Exception('Multiple parameters with different data types are not yet supported')
            pvpFormatStr = paramTypes[0]

        first = pvpFormatStr[0]
        if first in ['U', 'I', 'F']:  # Unsigned int, signed int, float
            return numpy.dtype(pvpFormatStr.lower())
        elif first == 'C':  # Complex float ('CF') or complex int ('CI')
            # This uses complex floats for both, which works but will take more space
            # TODO define a custom dtype for complex ints?
            return numpy.dtype('c' + pvpFormatStr[2:])
        elif first == 'S':  # String
            # TODO official format is “S[1-9][0-9]*”:
            #   Is the '*' literal or indicating that these can be however long?
            return numpy.dtype('U' + pvpFormatStr[1:])

        raise Exception('Unknown or unsupported format string: \'{0}\''.format(pvpFormatStr))

    def toListOfDicts(self, cphdMetadata):
        """
        \brief  Turns this PVPBlock object in a list of Python dictionaries with NumPy arrays
                of PVP data

        \param  cphdMetadata (SWIG-wrapped CPHD Metadata object)
                The metadata used to create this PVPBlock

        \return List of Python dictionaries containing NumPy arrays of PVP data.
                Each dictionary in the list corresponds to a CPHD data channel.
                The dictionary keys are string names of the PVP parameters in this PVPBlock
                    (specifically, the names of the attributes used to store them in a CPHD PVP
                    object, e.g. 'rcvTime').
                The dictionary values are NumPy arrays of shape
                    (cphdMetadata.getNumVectors(channel), cphdMetadata.getNumSamples(channel))
                    (with an extra dimension of size cphdMetadata.pvp.[param].getSize() if the
                    parameter size != 1).
                The data types of these arrays are set based on the PVP format string,
                    cphdMetadata.pvp.[param].getFormat(), using PVPBlock._pvpFormatToNPdtype()
        """

        # Determine which params need to be set
        paramsToCopy = dict(self.PVP_PARAM_GETTERS)  # Copy all required PVP params
        for optionalParam in self.OPTIONAL_PVP_PARAMS:
            # Call boolean `has[param]` method of PVPBlock to check if this PVPBlock has this param
            if getattr(self, self.OPTIONAL_PVP_PARAMS[optionalParam][0])():
                # Copy `get[param]` method into paramsToCopy
                paramsToCopy[optionalParam] = self.OPTIONAL_PVP_PARAMS[optionalParam][1]

        pvpData = []
        # Read data from each channel of this PVPBlock into list-of-dicts
        for channel in range(cphdMetadata.getNumChannels()):
            # Initialize dict of parameters for this channel
            channelPVP = {}
            for param in paramsToCopy:
                paramSize = getattr(cphdMetadata.pvp, param).getSize()
                paramShape = (cphdMetadata.getNumVectors(channel),)
                if paramSize != 1:
                    # If data is a vector, add another dimension to the array
                    paramShape += (paramSize,)
                paramDtype = self._pvpFormatToNPdtype(getattr(cphdMetadata.pvp, param).getFormat())
                channelPVP[param] = numpy.empty(shape=paramShape, dtype=paramDtype)

            # Copy PVP data for this channel by vector
            for vector in range(cphdMetadata.getNumVectors(channel)):
                for param in paramsToCopy:
                    # Call get[param]() method for current channel and vector
                    pulseVector = getattr(self, paramsToCopy[param])(channel, vector)
                    paramSize = getattr(cphdMetadata.pvp, param).getSize()
                    if paramSize == 1:
                        channelPVP[param][vector] = pulseVector
                    else:
                        for i in range(paramSize):
                            channelPVP[param][vector][i] = pulseVector[i]

            pvpData.append(channelPVP)

        return pvpData
%}
}

%extend cphd::Wideband
{
    // We need to expose a way to read into a raw buffer
    void readImpl(size_t channel,
                  size_t firstVector,
                  size_t lastVector,
                  size_t firstSample,
                  size_t lastSample,
                  size_t numThreads,
                  const types::RowCol<size_t>& dims,
                  long long data)
    {
        $self->read(channel,
                    firstVector,
                    lastVector,
                    firstSample,
                    lastSample,
                    numThreads,
                    dims,
                    reinterpret_cast<void*>(data));
    }
}

%extend cphd::CPHDReader
{
    PyObject* getPHD(size_t channel)
    {
        const auto& wb = self->getWideband();
        const types::RowCol<size_t> dims =
            wb.getBufferDims(channel,
                             0,
                             cphd::Wideband::ALL,
                             0,
                             cphd::Wideband::ALL);
        std::vector<std::complex<float>> buffer(dims.area());
        wb.read(channel,
                0,
                cphd::Wideband::ALL,
                0,
                cphd::Wideband::ALL,
                1,
                dims,
                buffer.data());
        return numpyutils::toNumpyArray(
            dims.row, dims.col, NPY_COMPLEX64, buffer.data());
    }
}

%extend cphd::CPHDWriter
{

%template(write) write<std::complex<float>>;

%pythoncode
%{
    def writeWideband(self, pvpBlock, widebands):
        """
        \brief  Write NumPy array(s) of wideband data to CPHD file

        \param  pvpBlock (cphd.PVPBlock)
        \param  widebands (np.ndarray OR list of np.ndarrays)
        """

        # Stack wideband arrays if they aren't already stacked
        if isinstance(widebands, list):
            import numpy as np
            contiguousWidebands = np.vstack(tuple(wideband for wideband in widebands))
        else:
            contiguousWidebands = widebands  # wideband is already contiguous
        # writeWidebandImpl() requires a single wideband array
        self.writeWidebandImpl(pvpBlock, contiguousWidebands, *contiguousWidebands.shape)

    def __del__(self):
        self.close()
%}

/*! Write NumPy array(s) of wideband data to CPHD file
 * \param PVPBlock (cphd::PVPBlock)
 * \param widebandArray
 *        A single NumPy array of wideband data (multiple channels are vstack()'d together)
 * \param rows
 *        Expected number of rows in widebandArray
 * \param cols
 *        Expected number of cols in widebandArray
 */
void writeWidebandImpl(PVPBlock pvpBlock, PyObject* widebandArray, size_t rows, size_t cols)
{
    // Verify that widebandArray is a complex64 NumPy array with the expected dimensions
    numpyutils::verifyArrayType(widebandArray, NPY_COMPLEX64);
    const types::RowCol<size_t> expectedWidebandDims(rows, cols);
    const types::RowCol<size_t> actualWidebandDims = numpyutils::getDimensionsRC(widebandArray);
    if (expectedWidebandDims != actualWidebandDims)
    {
        throw except::Exception("Wideband array dimensions ("
                                + std::to_string(actualWidebandDims.row) + ", "
                                + std::to_string(actualWidebandDims.col)
                                + ") do not match expected ("
                                + std::to_string(expectedWidebandDims.row) + ", "
                                + std::to_string(expectedWidebandDims.col) + ")");
    }

    std::complex<float>* widebandPtr = numpyutils::getBuffer<std::complex<float>>(widebandArray);
    self->write(pvpBlock, widebandPtr);  // write() is templated for complex<float> above
}

}

%pythoncode
%{
import numpy
import multiprocessing
from coda.coda_types import RowColSizeT

def read(self,
         channel = 0,
         firstVector = 0,
         lastVector = Wideband.ALL,
         firstSample = 0,
         lastSample = Wideband.ALL,
         numThreads = multiprocessing.cpu_count()):

    dims = self.getBufferDims(channel, firstVector, lastVector, firstSample, lastSample)
    sampleTypeSize = self.getElementSize()

    # RF32F_IM32F
    if sampleTypeSize == 8:
        dtype = 'complex64'
    else:
        raise Exception('Unknown element type')

    numpyArray = numpy.empty(shape = (dims.row, dims.col), dtype = dtype)
    pointer, ro = numpyArray.__array_interface__['data']
    self.readImpl(channel, firstVector, lastVector, firstSample, lastSample, numThreads, dims, pointer)
    return numpyArray

Wideband.read = read
%}

%extend cphd::CPHDXMLControl {
  cphd::Metadata fromXMLString(const std::string& xmlString)
  {
    cphd::Metadata retv;
    std::unique_ptr<cphd::Metadata> apMetadata = $self->fromXML(xmlString);
    retv = *apMetadata;
    return retv;
  }
}
%template(VectorVector2) std::vector<math::linear::VectorN<2,double> >;
%template(VectorVector3) std::vector<math::linear::VectorN<3,double> >;
%template(VectorString) std::vector<std::string>;
%template(VectorLineSample) std::vector<cphd::LineSample>;
%template(VectorSegment) std::vector<cphd::Segment>;
%template(VectorChannelParameters) std::vector<cphd::ChannelParameter>;
%template(VectorPoint) std::vector<cphd::Point>;
%template(VectorDataChannel) std::vector<cphd::Data::Channel>;
%template(VectorAntennaParameters) std::vector<six::sicd::AntennaParameters>;
%template(VectorSupportArrayParameter) std::vector<cphd::SupportArrayParameter>;
%template(VectorCOD) std::vector<cphd::COD>;
%template(VectorDwellTime) std::vector<cphd::DwellTime>;
%template(VectorAntCoordFrame) std::vector<cphd::AntCoordFrame>;
%template(VectorAntPhaseCenter) std::vector<cphd::AntPhaseCenter>;
%template(VectorAntPattern) std::vector<cphd::AntPattern>;
%template(VectorGainPhaseArray) std::vector<cphd::AntPattern::GainPhaseArray>;
%template(VectorTxParams) std::vector<cphd::TxWFParameters>;
%template(VectorRcvParams) std::vector<cphd::RcvParameters>;
%template(VectorCreationInfo) std::vector<cphd::ProductInfo::CreationInfo>;
%template(VectorGeoInfo) std::vector<six::GeoInfo>;

SCOPED_COPYABLE(cphd,SupportArray);
SCOPED_COPYABLE(cphd,TropoParameters);
SCOPED_COPYABLE(cphd,IonoParameters);
SCOPED_COPYABLE(cphd,Planar);
SCOPED_COPYABLE(cphd,HAE);
SCOPED_COPYABLE(cphd,AreaType);
SCOPED_COPYABLE(cphd,ImageGrid);
SCOPED_COPYABLE(cphd,TxRcv);
SCOPED_COPYABLE(cphd,TOAExtended);
SCOPED_COPYABLE_RENAME(cphd,TOAExtended::LFMEclipse,TOALFMEclipse);
SCOPED_COPYABLE_RENAME(cphd,ChannelParameter::Antenna,ChannelAntenna);
SCOPED_COPYABLE_RENAME(cphd,ChannelParameter::TxRcv,ChannelTxRcv);
SCOPED_COPYABLE(cphd,TgtRefLevel);
SCOPED_COPYABLE(cphd,NoiseLevel);
SCOPED_COPYABLE(cphd,FxNoiseProfile);
SCOPED_COPYABLE(cphd,Monostatic);
SCOPED_COPYABLE(cphd,Bistatic);
SCOPED_COPYABLE(cphd,ErrorParameters);
SCOPED_COPYABLE_RENAME(cphd,ErrorParameters::Monostatic,ErrorMonostatic);
SCOPED_COPYABLE(six,DecorrType);
SCOPED_COPYABLE(cphd,ProductInfo);
SCOPED_COPYABLE(cphd,MatchInformation);
SCOPED_COPYABLE_RENAME(cphd,Antenna,CphdAntenna);
