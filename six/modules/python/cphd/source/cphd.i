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
