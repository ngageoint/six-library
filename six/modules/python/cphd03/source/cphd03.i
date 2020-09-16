/*
 * =========================================================================
 * This file is part of cphd03-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd03-python is free software; you can redistribute it and/or modify
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

%module(package="pysix") cphd03

%feature("autodoc", "1");

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
#include "import/cphd03.h"
#include "import/six.h"
#include "import/six/sicd.h"
#include "import/sys.h"
using six::Vector3;
%}
%ignore cphd03::CPHDXMLControl::toXML(const Metadata& metadata);
%ignore cphd03::CPHDXMLControl::fromXML(const xml::lite::Document* doc);
%ignore cphd03::CPHDXMLControl::fromXML(const std::string& xmlString);
%rename(CphdAntenna) cphd03::Antenna;


%include "cphd/Types.h"
%include "cphd/Enums.h"
%include "cphd/MetadataBase.h"
%include "cphd03/FileHeader.h"
%include "cphd03/Global.h"
%include "cphd03/Channel.h"
%include "cphd03/SRP.h"
%include "cphd03/Antenna.h"
%include "cphd03/VectorParameters.h"
%include "cphd03/Metadata.h"
%include "cphd03/Data.h"
%include "cphd03/VBM.h"
%include "cphd03/CPHDXMLControl.h"

%include "cphd/Wideband.h"
%include "cphd03/CPHDReader.h"
%include "cphd03/CPHDWriter.h"

%ignore CPHDWriter::writeCPHDData;
%ignore CPHDWriter::close;
%ignore CPHDWriter::writeMetadata;
%ignore CPHDWriter::addImage;
%ignore CPHDWriter::write;

%extend cphd03::FileHeader
{
    std::string __str__()
    {
        std::ostringstream out;
        out << *$self;
        return out.str();
    }
}

%extend cphd03::Metadata
{
    std::string __str__()
    {
        std::ostringstream out;
        out << *$self;
        return out.str();
    }
}

%extend cphd03::VBM
{
    std::string __str__()
    {
        std::ostringstream out;
        out << *$self;
        return out.str();
    }
}

%extend cphd03::VBM
{
    void getVBMdata(size_t channel, size_t data)
    {
        void* buffer = reinterpret_cast<void*>(data);
        $self->getVBMdata(channel, buffer);
    }
}

// NOTE: In the cases below, need to use 'long long' rather
//       than 'size_t' for things involving pointers.
//       Otherwise, Swig will generate code using
//       PyInt_FromLong().  This will work fine on
//       64-bit Unix where sizeof(long) == 8 and works fine
//       on 64-bit Windows where sizeof(long) == 4... until
//       a value gets too large to represent in 4 bytes at
//       which point you'll get cryptic/confusing runtime
//       errors.  This happens in particular when trying to
//       send NumPy arrays to/from C++ when you allocate an
//       array > 4 GB.  It seems like Swig should be smarter
//       in what it auto-generates to avoid this.
%extend cphd03::CPHDWriter
{
    void addImageImpl(long long image,
            const types::RowCol<size_t>& dims,
            long long vbm)
    {
        $self->addImage(reinterpret_cast<std::complex<float>*>(image),
                dims,
                reinterpret_cast<sys::ubyte*>(vbm));
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

%extend cphd::CPHDWriter
{
%pythoncode
%{
    def __del__(self):
        self.close()
%}
}

%pythoncode
%{
import numpy
import multiprocessing
from coda.coda_types import RowColSizeT

def toBuffer(self, channel = 0):
    numpyArray = numpy.empty(shape = int((self.getVBMsize(channel) / 8)),
                             dtype = 'double')
    pointer, ro = numpyArray.__array_interface__['data']

    self.getVBMdata(channel, pointer)
    return numpyArray

VBM.toBuffer = toBuffer

def write(self, pathname, data, vbm, channel):
    '''
    Write CPHD data to a file

    Args:
        pathname: Name of file to write to
        data: Numpy array of complex64 wideband data of size [npulse, nsamps]
        vbm: VBM object
        channel: Channel of VBM to write

    Throws:
        TypeError if trying to write non-complex64 wideband data
    '''
    if data.dtype != numpy.dtype('complex64'):
        raise TypeError('Python CPHDWriter only supports complex float data')

    imagePointer, _ = data.__array_interface__['data']
    vbmBuffer = vbm.toBuffer(channel)
    vbmPointer, _ = vbmBuffer.__array_interface__['data']

    dims = RowColSizeT(data.shape[0], data.shape[1])
    self.addImageImpl(imagePointer, dims, vbmPointer)
    self.write()

CPHDWriter.writeCPHD = write

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

%extend cphd03::CPHDXMLControl {
  cphd03::Metadata fromXMLString(const std::string& xmlString)
  {
    cphd03::Metadata retv;
    auto apMetadata = $self->fromXML(xmlString);
    retv = *apMetadata;
    return retv;
  }
}
%template(VectorArraySize) std::vector<cphd03::ArraySize>;
%template(VectorVector3) std::vector<math::linear::VectorN<3,double> >;
%template(VectorChannelParameters) std::vector<cphd03::ChannelParameters>;
%template(VectorAntennaParameters) std::vector<six::sicd::AntennaParameters>;


SCOPED_COPYABLE(cphd03,DwellTimeParameters);
SCOPED_COPYABLE(cphd03,AreaPlane);
SCOPED_COPYABLE(cphd03,FxParameters);
SCOPED_COPYABLE(cphd03,TOAParameters);
SCOPED_COPYABLE_RENAME(cphd03,Antenna,CphdAntenna);

