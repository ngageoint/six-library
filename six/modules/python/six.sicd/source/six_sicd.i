/*
 * =========================================================================
 * This file is part of six.sicd-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * six.sicd-python is free software; you can redistribute it and/or modify
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

%module(package="pysix") six_sicd

%feature("autodoc", "1");

%{

#include <complex>
#include <utility>

#include "import/mem.h"
#include "import/six.h"
#include "import/six/sicd.h"
#include "six/sicd/SICDWriteControl.h"
#include <numpyutils/numpyutils.h>

using namespace six::sicd;
using namespace six;

six::sicd::ComplexData * asComplexData(six::Data* data);

/* Need this because we can't really do it on the python side of things */
six::sicd::ComplexData * asComplexData(six::Data* data)
{
  six::sicd::ComplexData * complexData = dynamic_cast<six::sicd::ComplexData*>(data);
  if( !complexData )
  {
    throw except::BadCastException();
  }
  else
  {
    return complexData;
  }
}

void writeNITF(const std::string& pathname, const std::vector<std::string>&
        schemaPaths, const six::sicd::ComplexData& data, long long imageAdr);

void writeNITF(const std::string& pathname, const std::vector<std::string>&
        schemaPaths, const six::sicd::ComplexData& data, long long imageAdr)
{
    const std::complex<float>* image = reinterpret_cast<
            std::complex<float>* >(imageAdr);

    six::XMLControlFactory::getInstance().addCreator(
            six::DataType::COMPLEX,
            new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

    mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::COMPLEX));
    std::auto_ptr<logging::Logger> logger(logging::setupLogger("out"));

    container->addData(data.clone());

    six::NITFWriteControl writer;
    writer.initialize(container);
    writer.setLogger(logger.get());

    six::BufferList buffers;
    buffers.push_back(reinterpret_cast<const six::UByte*>(image));

    writer.save(buffers, pathname, schemaPaths);
}

%}
%ignore mem::ScopedCloneablePtr::operator!=;
%ignore mem::ScopedCloneablePtr::operator==;
%ignore mem::ScopedCopyablePtr::operator!=;
%ignore mem::ScopedCopyablePtr::operator==;

%include <std_vector.i>
%include <std_string.i>
%include <std_complex.i>
%include <std_pair.i>
%include <std_auto_ptr.i>

%import "math_poly.i"
%import "six.i"
%import "io.i"
%import "mem.i"

// This allows functions that return auto_ptrs to work properly
%auto_ptr(six::sicd::ComplexData);

/* wrap that function defined in the header section */
six::sicd::ComplexData * asComplexData(six::Data* data);

void writeNITF(const std::string& pathname, const std::vector<std::string>&
        schemaPaths, const six::sicd::ComplexData& data, long long imageAdr);

%pythoncode
%{
import os
import sys

def schema_path():
    """Provide an absolute path to the schemas."""
    try:
        pysix_path = os.path.dirname(__file__)
    except NameError:
        # Must be running as __main__, so use sys.argv
        pysix_path = os.path.dirname(sys.argv[0])
    return os.path.abspath(os.path.join(pysix_path, 'schemas'))
%}

/* prevent name conflicts */
%rename ("SixSicdUtilities") six::sicd::Utilities;

%include "six/sicd/ComplexClassification.h"
%include "six/sicd/CollectionInformation.h"
%include "six/sicd/ImageCreation.h"
%include "six/sicd/ImageData.h"
%include "six/sicd/GeoData.h"
%include "six/sicd/Grid.h"
%include "six/sicd/Timeline.h"
%include "six/sicd/Position.h"
%include "six/sicd/RadarCollection.h"
%include "six/sicd/ImageFormation.h"
%include "six/sicd/SCPCOA.h"
%include "six/sicd/Antenna.h"
%include "six/sicd/MatchInformation.h"
%include "six/sicd/PFA.h"
%include "six/sicd/RMA.h"
%include "six/sicd/RgAzComp.h"
%include "six/sicd/ComplexData.h"
%include "six/sicd/ComplexXMLControl.h"
%include "six/sicd/Utilities.h"

/* We need this because SWIG cannot do it itself, for some reason */
/* TODO: write script to generate all of these instantiations for us? */

SCOPED_CLONEABLE(six::sicd, CollectionInformation)
SCOPED_CLONEABLE(six::sicd, ImageCreation)
SCOPED_COPYABLE(six::sicd, ImageData)
SCOPED_CLONEABLE(six::sicd, GeoData)
SCOPED_CLONEABLE(six::sicd, Grid)
SCOPED_COPYABLE(six::sicd, Timeline)
SCOPED_COPYABLE(six::sicd, Position)
SCOPED_COPYABLE(six::sicd, RcvAPC)
SCOPED_CLONEABLE(six::sicd, RadarCollection)
SCOPED_COPYABLE(six::sicd, ImageFormation)
SCOPED_COPYABLE(six::sicd, SCPCOA)
SCOPED_COPYABLE(six::sicd, Antenna)
SCOPED_COPYABLE(six::sicd, MatchInformation)
SCOPED_COPYABLE(six::sicd, SlowTimeDeskew)
SCOPED_COPYABLE(six::sicd, PFA)
SCOPED_COPYABLE(six::sicd, RMA)
SCOPED_COPYABLE(six::sicd, RgAzComp)

SCOPED_CLONEABLE(six::sicd, GeoInfo)
%template(VectorScopedCloneableGeoInfo) std::vector<mem::ScopedCloneablePtr<six::sicd::GeoInfo> >;
%template(VectorLatLon) std::vector<scene::LatLon>;

SCOPED_COPYABLE(six::sicd, AntennaParameters)
SCOPED_COPYABLE(six::sicd, ElectricalBoresight)
SCOPED_COPYABLE(six::sicd, HalfPowerBeamwidths)
SCOPED_COPYABLE(six::sicd, GainAndPhasePolys)

SCOPED_COPYABLE(six::sicd, MatchType)
SCOPED_COPYABLE(six::sicd, WeightType)

%template(VectorPolyXYZ) std::vector<math::poly::OneD<Vector3> >;

SCOPED_CLONEABLE(six::sicd, DirectionParameters)

SCOPED_CLONEABLE(six::sicd, AreaPlane)
SCOPED_CLONEABLE(six::sicd, AreaDirectionParameters)
SCOPED_CLONEABLE(six::sicd, Segment)
SCOPED_CLONEABLE(six::sicd, TxStep)
SCOPED_CLONEABLE(six::sicd, WaveformParameters)
SCOPED_CLONEABLE(six::sicd, Area)
SCOPED_CLONEABLE(six::sicd, ChannelParameters)
%template(VectorScopedCloneableWaveformParameters) std::vector<mem::ScopedCloneablePtr<six::sicd::WaveformParameters> >;
%template(VectorScopedCloneableTxStep)             std::vector<mem::ScopedCloneablePtr<six::sicd::TxStep> >;
%template(vectorScopedClonableSegment)             std::vector<mem::ScopedCloneablePtr<six::sicd::Segment> >;
%template(VectorScopedCloneableChannelParameters)  std::vector<mem::ScopedCloneablePtr<six::sicd::ChannelParameters> >;
%template(VectorInt)                               std::vector<int>;
SCOPED_COPYABLE(six::sicd, RcvChannelProcessed)
%template(VectorProcessing)                        std::vector<six::sicd::Processing>;
SCOPED_COPYABLE(six::sicd, PolarizationCalibration)
SCOPED_COPYABLE(six::sicd, Distortion)

%template(VectorMatchCollect)                      std::vector<six::sicd::MatchCollect>;
%template(VectorScopedCopyableMatchType)           std::vector<mem::ScopedCopyablePtr<six::sicd::MatchType> >;

SCOPED_COPYABLE(six::sicd, RMAT)
SCOPED_COPYABLE(six::sicd, RMCR)
SCOPED_COPYABLE(six::sicd, INCA)

SCOPED_COPYABLE(six::sicd, InterPulsePeriod)
%template(VectorTimelineSet)                       std::vector<six::sicd::TimelineSet>;

// NOTE: In the cases below, need to use 'long long' rather
//       than 'size_t'.  Otherwise, Swig will generate code
//       using PyInt_FromLong().  This will work fine on
//       64-bit Unix where sizeof(long) == 8 and works fine
//       on 64-bit Windows where sizeof(long) == 4... until
//       a value gets too large to represent in 4 bytes at
//       which point you'll get cryptic/confusing runtime
//       errors.  This happens in particular when trying to
//       send NumPy arrays to/from C++ when you allocate an
//       array > 4 GB.  It seems like Swig should be smarter
//       in what it auto-generates to avoid this.
%{
    void getWidebandRegion(const std::string& sicdPathname,
            const std::vector<std::string>& schemaPaths,
            six::sicd::ComplexData* complexData,
            long long startRow, long long numRows,
            long long startCol, long long numCols, long long arrayBuffer)
    {
        std::complex<float>* realBuffer =
                reinterpret_cast< std::complex<float>* >(arrayBuffer);

        types::RowCol<size_t> offset(startRow, startCol);
        types::RowCol<size_t> extent(numRows, numCols);
        Utilities::getWidebandData(sicdPathname, schemaPaths,
                *complexData, offset, extent, realBuffer);
    }
%}

void getWidebandRegion(std::string sicdPathname,
        const std::vector<std::string>& schemaPaths,
        six::sicd::ComplexData* complexData,
        long long startRow, long long numRows,
        long long startCol, long long numCols, long long arrayBuffer);

%pythoncode %{
import numpy as np
from pysix.six_base import VectorString
from coda.coda_io import FileOutputStream
from coda.xml_lite import *

def writeAsNITF(outFile, schemaPaths, complexData, image):
    writeNITF(outFile, schemaPaths, complexData,
        image.__array_interface__["data"][0])

%}

%include "six/sicd/SICDWriteControl.h"
%extend six::sicd::SICDWriteControl
{
    void write(PyObject* data, const types::RowCol<size_t>& offset)
    {
        numpyutils::verifyArrayType(data, NPY_COMPLEX64);

        // TODO: Force array to be contigious memory
        //       Right now we're requiring the caller to do that
        // TODO: If we get noncontiguous memory, maybe we want to
        //       instead do multiple calls to save() ourselves to
        //       avoid the memory allocation
        $self->save(numpyutils::getBuffer<std::complex<float> >(data),
                    offset,
                    numpyutils::getDimensionsRC(data));
    }

    void initXMLControlRegistry(six::XMLControlRegistry& xmlRegistry)
    {
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                               new six::XMLControlCreatorT<
                                       six::sicd::ComplexXMLControl>());
        $self->setXMLControlRegistry(&xmlRegistry);
    }
}
