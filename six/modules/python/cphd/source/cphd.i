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

%include "std_map.i"
%template(MapStringAPVPType) std::map<std::string, cphd::APVPType>;

%extend cphd::PVPBlock
{
// Types from Table 10-2 of the CPHD spec
%template(getUnsignedIntAddedPVP) getAddedPVP<unsigned>;
%template(getIntAddedPVP) getAddedPVP<int>;
%template(getFloatAddedPVP) getAddedPVP<float>;
%template(getComplexSignedIntAddedPVP) getAddedPVP<std::complex<int> >;
%template(getComplexFloatAddedPVP) getAddedPVP<std::complex<float> >;
%template(getStringAddedPVP) getAddedPVP<std::string>;

%template(setUnsignedIntAddedPVP) setAddedPVP<unsigned>;
%template(setIntAddedPVP) setAddedPVP<int>;
%template(setFloatAddedPVP) setAddedPVP<float>;
%template(setComplexSignedIntAddedPVP) setAddedPVP<std::complex<int> >;
%template(setComplexFloatAddedPVP) setAddedPVP<std::complex<float> >;
%template(setStringAddedPVP) setAddedPVP<std::string>;
}

%extend cphd::PVPBlock
{
%pythoncode
%{
    # Map PVPBlock field names to how they're used in the get/set methods for each PVP parameter
    # in a CPHD PVPBlock object.  We could generate these programmatically, but the
    # upper/lowercasing of the method names is not 100% consistent (TOAE, TdIonoSRP)
    # and this is very explicit
    PVP_PARAM_METHODS = {
        'txTime': 'TxTime',
        'txPos': 'TxPos',
        'txVel': 'TxVel',
        'rcvTime': 'RcvTime',
        'rcvPos': 'RcvPos',
        'rcvVel': 'RcvVel',
        'srpPos': 'SRPPos',
        'aFDOP': 'aFDOP',
        'aFRR1': 'aFRR1',
        'aFRR2': 'aFRR2',
        'fx1': 'Fx1',
        'fx2': 'Fx2',
        'toa1': 'TOA1',
        'toa2': 'TOA2',
        'tdTropoSRP': 'TdTropoSRP',
        'sc0': 'SC0',
        'scss': 'SCSS'
    }
    OPTIONAL_PVP_PARAMS = {
        'ampSF': ('hasAmpSF', 'AmpSF'),
        'fxN1': ('hasFxN1', 'FxN1'),
        'fxN2': ('hasFxN2', 'FxN2'),
        'signal': ('hasSignal', 'Signal'),
        'tdIonoSRP': ('hasTDIonoSRP', 'TdIonoSRP'),
        'toaE1': ('hasToaE1', 'TOAE1'),
        'toaE2': ('hasToaE2', 'TOAE2')
    }

    import numpy  # 'as np' doesn't work unless the import is within each function

    @staticmethod
    def _validateMultiplePVPFormatStr(pvpFormatStr):
        """
        \brief  Confirms that a valid PVP format string (CPHD Spec Table 10-2) with multiple
                parameters (e.g. 'X=U2;Y=U2;') uses the same data type for all parameters.
                Returns the data type if it is the same for all parameters, raises an exception
                otherwise

        \param  pvpFormatStr (str)
                CPHD PVP format string, with multiple parameters e.g. 'X=U1;Y=U1;'
                See CPHD Spec Table 10-2

        \return Data type if it is the same for all parameters, raises an exception otherwise
        """

        paramTypes = [param[param.index('=') + 1:] for param in pvpFormatStr.split(';') if param]

        # TODO support multiple different parameter types ('A=U2;B=I2;')
        if not all(paramType == paramTypes[0] for paramType in paramTypes[1:]):
            raise Exception('Multiple parameters with different data types are not yet supported')

        return paramTypes[0]

    @staticmethod
    def _pvpFormatToNPdtype(pvpFormatStr):
        """
        \brief  Maps valid PVP format strings (CPHD Spec Table 10-2) to NumPy dtypes
                Currently doesn't support multiple parameters with different types,
                e.g. 'X=U2;Y=F4;'

        \param  pvpFormatStr (str)
                CPHD PVP format string, e.g. 'U1' or 'CI2'. See CPHD Spec Table 10-2

        \return NumPy dtype corresponding to pvpFormatStr
        """

        if '=' in pvpFormatStr and ';' in pvpFormatStr:
            # This string has multiple parameters, assert that they are all the same data type
            pvpFormatStr = PVPBlock._validateMultiplePVPFormatStr(pvpFormatStr)

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

    def pvpFormatToAddedPVPMethod(self, getOrSet, pvpFormatStr):
        """
        \brief  Returns a callable method object to get or set an added PVP. PVPBlock.getAddedPVP()
                is templated based on the type of the parameter it returns, so we need to get the
                correct method name for the type of the parameter

        \param  getOrSet (str)
                'get' or 'set', depending on whether we want the getter or setter method for this
                type of PVP
        \param  pvpFormatStr (str)
                CPHD PVP format string, e.g. 'U1' or 'CI2'. See CPHD Spec Table 10-2

        \return Callable method object to get or set an added PVP for this PVPBlock
        """

        if '=' in pvpFormatStr and ';' in pvpFormatStr:
            # This string has multiple parameters, assert that they are all the same data type
            pvpFormatStr = PVPBlock._validateMultiplePVPFormatStr(pvpFormatStr)

        if getOrSet not in ['get', 'set']:
            raise Exception('getOrSet should be only \'get\' or \'set\', not {}'.format(getOrSet))

        methodName = None
        if pvpFormatStr.startswith('U'):
            methodName = 'UnsignedIntAddedPVP'
        elif pvpFormatStr.startswith('I'):
            methodName = 'IntAddedPVP'
        elif pvpFormatStr.startswith('F'):
            methodName = 'FloatAddedPVP'
        elif pvpFormatStr.startswith('CI'):
            methodName = 'ComplexSignedIntAddedPVP'
        elif pvpFormatStr.startswith('CF'):
            methodName = 'ComplexFloatAddedPVP'
        elif pvpFormatStr.startswith('S'):
            methodName = 'StringAddedPVP'

        return getattr(self, getOrSet + methodName)

    def getDefaultParametersInUse(self):
        """
        \brief  Returns a dict mapping PVPBlock field names for the default PVP parameters
                in this block to the names used in their get/set methods. Method names will
                need to have 'get' or 'set' prepended

        \return A dict mapping field names for the default PVP parameters used in this block
                to the names used in their get/set methods
        """

        # Determine which (non-custom) params need to be set
        usedParams = dict(self.PVP_PARAM_METHODS)  # Copy all required PVP params
        for optionalParam in self.OPTIONAL_PVP_PARAMS:
            # Call boolean `has[param]` method of PVPBlock to check if this PVPBlock has this param
            if getattr(self, self.OPTIONAL_PVP_PARAMS[optionalParam][0])():
                # Copy `get[param]` method name into usedParams
                usedParams[optionalParam] = self.OPTIONAL_PVP_PARAMS[optionalParam][1]
        return usedParams

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
                Any added PVP parameters should also have been added to cphdMetadata.pvp.addedPVP
        """

        # getDefaultParametersInUse() maps all string param names to the names used in their
        # get/set methods. Call it and reorganize a little: prepend 'get' and wrap the string
        # method names in tuples
        paramsToCopy = {paramName: ('get' + paramMethodName,)
                        for paramName, paramMethodName in self.getDefaultParametersInUse().items()}

        for paramName in paramsToCopy:
            # Append PVPType object to tuple inside paramsToCopy
            paramsToCopy[paramName] += (getattr(cphdMetadata.pvp, paramName),)
        # Add custom PVP objects, which don't have getters
        # (APVPType ("AddedPVPType") derives from PVPType)
        paramsToCopy.update({paramName: (None, paramObj)
                             for paramName, paramObj in cphdMetadata.pvp.addedPVP.items()})

        # Now paramsToCopy consists of:
        # {'param1Name': ('getParam1', cphd.PVPType object for param1)}
        # for all default and custom PVP parameters used in this PVPBlock

        pvpData = []
        # Read data from each channel of this PVPBlock into list-of-dicts
        for channel in range(cphdMetadata.getNumChannels()):
            # Initialize dict of parameters for this channel
            # Doing this for each channel in case they have different numbers of vectors/samples
            channelPVP = {}
            for paramName, (paramGetter, paramObj) in paramsToCopy.items():
                paramSize = paramObj.getSize()
                paramShape = (cphdMetadata.getNumVectors(channel),)
                if paramSize != 1:
                    # If data is a vector, add another dimension to the array
                    paramShape += (paramSize,)
                paramDtype = self._pvpFormatToNPdtype(paramObj.getFormat())
                channelPVP[paramName] = numpy.empty(shape=paramShape, dtype=paramDtype)

            # Copy PVP data for this channel by vector
            for vector in range(cphdMetadata.getNumVectors(channel)):
                for paramName, (paramGetter, paramObj) in paramsToCopy.items():
                    # Get, then call, the PVPBlock.get[param]() method object for
                    # current channel and vector (or use PVPBlock.getAddedPVP() if
                    # this is a custom parameter)
                    pulseVector = (getattr(self, paramGetter)(channel, vector)
                                   if paramName not in cphdMetadata.pvp.addedPVP else
                                   self.pvpFormatToAddedPVPMethod('get', paramObj.getFormat())(
                                        channel, vector, paramName))
                    paramSize = paramObj.getSize()
                    if paramSize == 1:
                        channelPVP[paramName][vector] = pulseVector
                    else:
                        for i in range(paramSize):
                            channelPVP[paramName][vector][i] = pulseVector[i]

            pvpData.append(channelPVP)

        return pvpData

    @staticmethod
    def fromListOfDicts(pvpData, cphdMetadata):
        """
        \brief  Initializes a PVPBlock using provided CPHD metadata and populates it from a list
                of Python dicts

        \param  pvpData (list of Python dicts)
                List of Python dicts (one for each channel) mapping parameter names
                to NumPy arrays of data.  See PVPBlock.toListOfDicts() for more information
                on the structure expected here
        \param  cphdMetadata (SWIG-wrapped CPHD Metadata object)
                The metadata used to create this PVPBlock
        """

        pvpBlock = PVPBlock(cphdMetadata.pvp, cphdMetadata.data)  # Call other PVPBlock constructor

        paramsToSet = {paramName: 'set' + paramMethodName for
                       paramName, paramMethodName in pvpBlock.getDefaultParametersInUse().items()}

        # For each parameter, check that all actual data sizes equal metadata size
        expectedParamSizes = {**{paramName: getattr(cphdMetadata.pvp, paramName).getSize()
                                 for paramName in paramsToSet},
                              **{paramName: paramObj.getSize()
                                 for paramName, paramObj in cphdMetadata.pvp.addedPVP.items()}}
        mismatchedParams = []  # Parameters for which actual size doesn't match size in metadata
        for paramName in (list(paramsToSet.keys()) + list(cphdMetadata.pvp.addedPVP.keys())):
            for channelIndex, channelData in enumerate(pvpData):
                actualSize = 1 if len(channelData[paramName].shape) == 1 \
                               else channelData[paramName].shape[1]
                if actualSize != expectedParamSizes[paramName]:
                    mismatchedParams.append(
                        (paramName, channelIndex, actualSize, expectedParamSizes[paramName]))
        if mismatchedParams:
            raise Exception('For the following parameters, actual data size does not match size '
                            + 'in metadata object:\n'
                            + '\n'.join(
                                ['Parameter: {0}  Channel: {1}  Actual: {2}  Expected: {3}'
                                 .format(param, channel, actual, expected)
                                 for (param, channel, actual, expected) in mismatchedParams]))

        # Populate PVPBlock object from pvpData
        for channelIndex, channelData in enumerate(pvpData):
            for vectorIndex in range(cphdMetadata.getNumVectors(channelIndex)):
                for paramName, data in channelData.items():
                    paramData = data[vectorIndex]
                    if isinstance(paramData, numpy.ndarray):
                        if len(paramData) == 1:
                            # Turn 1D array into scalar so we can set it below
                            # Note that, if reading this PVPBlock back into a list of dicts, this
                            # parameter will have one less dimension than was originally passed in,
                            # since we removed it here
                            paramData = paramData.item()
                        # Could use Vector2 here, but there aren't any size 2 default parameters
                        elif len(paramData) == 3:
                            paramData = coda.math_linear.Vector3(paramData)
                        else:
                            raise Exception(('Only PVP parameters of size 1 or 3 are supported, '
                                             + '\'{0}\' has size {1}'
                                             .format(paramName, len(paramData))))
                    if paramName not in cphdMetadata.pvp.addedPVP:
                        # Get the setter method for this parameter, then call it with indices and
                        # data to set for this parameter
                        getattr(pvpBlock, paramsToSet[paramName])(paramData, channelIndex, vectorIndex)
                    else:
                        # Get and call setter method for the type of this custom parameter
                        pvpBlock.pvpFormatToAddedPVPMethod('set', cphdMetadata.pvp.addedPVP[paramName].getFormat())(
                            paramData, channelIndex, vectorIndex, paramName)
        return pvpBlock
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
    def __del__(self):
        self.close()
%}

/*! Write NumPy array of wideband data to CPHD file
 * \param PVPBlock (cphd::PVPBlock)
 * \param widebandArray
 *        A single, contiguous NumPy array of all wideband data
          Multiple data channels should be vstack'd together (first channel first)
 * \param rows
 *        Expected number of rows in widebandArray
 * \param cols
 *        Expected number of cols in widebandArray
 */
void writeWideband(PVPBlock pvpBlock, PyObject* widebandArray, size_t rows, size_t cols)
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
