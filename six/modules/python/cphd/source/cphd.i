/*
 * =========================================================================
 * This file is part of cphd-python 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
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
#include "import/cphd.h"
#include "import/six.h"
#include "import/six/sicd.h"
using six::Vector3;
%}
%ignore cphd::CPHDXMLControl::toXML(const Metadata& metadata);
%ignore cphd::CPHDXMLControl::fromXML(const xml::lite::Document* doc);
%ignore cphd::CPHDXMLControl::fromXML(const std::string& xmlString);
%rename(CphdAntenna) cphd::Antenna;

%include "cphd/Types.h"
%include "cphd/Enums.h"
%include "cphd/FileHeader.h"
%include "cphd/Global.h"
%include "cphd/Channel.h"
%include "cphd/SRP.h"
%include "cphd/Antenna.h"
%include "cphd/VectorParameters.h"
%include "cphd/Metadata.h"
%include "cphd/Data.h"
%include "cphd/VBM.h"
%include "cphd/CPHDXMLControl.h"

%include "cphd/Wideband.h"
%include "cphd/CPHDReader.h"

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

%extend cphd::VBM
{
    std::string __str__()
    {
        std::ostringstream out;
        out << *$self;
        return out.str();
    }
}

%extend cphd::VBM
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

%pythoncode
%{
import numpy

def toBuffer(self, channel = 0):
    
    numpyArray = numpy.empty(shape = ((self.getVBMsize(channel) / 8)), dtype = 'double')
    pointer, ro = numpyArray.__array_interface__['data']
    
    self.getVBMdata(channel, pointer)
    return numpyArray
    
VBM.toBuffer = toBuffer
%}

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
    sampleType = self.getSampleType()
    
    # RF32F_IM32F
    if sampleType == 1:
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
    std::auto_ptr<cphd::Metadata> apMetadata = $self->fromXML(xmlString);
    retv = *apMetadata;
    return retv;
  }
}
%template(VectorArraySize) std::vector<cphd::ArraySize>;
%template(VectorVector3) std::vector<math::linear::VectorN<3,double> >;
%template(VectorChannelParameters) std::vector<cphd::ChannelParameters>;
%template(VectorAntennaParameters) std::vector<six::sicd::AntennaParameters>;


SCOPED_COPYABLE(cphd,DwellTimeParameters);
SCOPED_COPYABLE(cphd,AreaPlane);
SCOPED_COPYABLE(cphd,FxParameters);
SCOPED_COPYABLE(cphd,TOAParameters);
SCOPED_COPYABLE_RENAME(cphd,Antenna,CphdAntenna);
