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

%include "std_string.i"
%include "std_vector.i"

%import "types.i"
%import "mem.i"
%import "six_sicd.i"
%import "six.i"
%import "math_poly.i"
%import "math_linear.i"
%import "scene.i"

%{
#include "import/cphd.h"
%}


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
                  size_t data)
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

%template(VectorArraySize) std::vector<cphd::ArraySize>;
%template(VectorVector3) std::vector<six::Vector3>;
%template(VectorPolyXYZ) std::vector<six::PolyXYZ>;

SCOPED_COPYABLE(cphd,DwellTimeParameters);
SCOPED_COPYABLE(cphd,AreaPlane);
SCOPED_COPYABLE(cphd,FxParameters);
SCOPED_COPYABLE_RENAME(cphd,Antenna,CphdAntenna);

