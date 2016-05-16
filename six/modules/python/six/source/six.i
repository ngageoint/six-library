/* =========================================================================
 * This file is part of six-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * six-python is free software; you can redistribute it and/or modify
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

%module(package="pysix") six_base

%feature("autodoc","1");

%include "std_vector.i"
%include "std_string.i"

%{

#include <memory>
#include <vector>
#include <cstddef>
using std::ptrdiff_t;

#include "import/six.h"

/* this isn't imported by the above include */
#include "six/Radiometric.h"
#include "six/MatchInformation.h"

#include "import/nitf.hpp"

using namespace six;

six::Data * parseDataNoAutoPtr(const XMLControlRegistry& xmlReg,
                      ::io::InputStream& xmlStream,
                      DataType dataType,
                      const std::vector<std::string>& schemaPaths,
                      logging::Logger& log)
{
  std::auto_ptr<Data> retv = six::parseData(xmlReg, xmlStream, dataType, schemaPaths, log);
  return retv.release();
}

%}

%ignore mem::ScopedCopyablePtr::operator!=;
%ignore mem::ScopedCopyablePtr::operator==;
%ignore mem::ScopedCloneablePtr::operator!=;
%ignore mem::ScopedCloneablePtr::operator==;

%import "types.i"
%import "except.i"
%import "math_poly.i"
%import "math_linear.i"
%import "scene.i"
%import "mem.i"

/* parametric elt-size array */
/* will probably want it eventually, but it looks like six.sicd doesn't use it */
%ignore "LUT";

/* auto_ptr causes problems, as well as
 * xml factory stuff that we'll just
 * put aside for now
 */
%ignore parseData;

/* ignore some useless (in Python) functions in ParameterCollection */
%ignore six::ParameterCollection::begin;
%ignore six::ParameterCollection::end;


/* prevent name conflicts */
%rename("SixUtilities") six::Utilities;


/* current six python interface consists of these files */
%include "nitf/DateTime.hpp"
%include "six/Enums.h"
%include "six/Types.h"
%include "six/Init.h"
%include "six/Parameter.h"
%include "six/ParameterCollection.h"
%include "six/Classification.h"
%include "six/ErrorStatistics.h"
%include "six/Radiometric.h"
%include "six/MatchInformation.h"
%include "six/Data.h"
%include "six/XMLControl.h"
%include "six/Utilities.h"
%include "six/Options.h"

%feature("shadow") six::Parameter::setValue(const std::string &)
%{
def setValue(self, *args):
    if len(args) != 1:
        raise RuntimeError("Parameter.setValue takes exactly one argument")
    new_tuple = (str(args[0]), )
    return $action(self, str(args[0]))
%}


%extend six::Parameter
{

  public:
    void setValue(const std::string & str)
    {
      $self->setValue<std::string>(str);
    }
    std::string __str__()
    {
      return $self->str();
    }
    long __int__()
    {
      return str::toType<long>($self->str());
    }
    double __float__()
    {
      return str::toType<double>($self->str());
    }
};

%extend six::ParameterCollection
{
  public:
    six::Parameter& __getitem__(size_t i)
    {
      return (*$self)[i];
    }
    void __setitem__(size_t i, const six::Parameter & v)
    {
      (*$self)[i] = v;
    }
};

/* We need this because SWIG cannot do it itself, for some reason */
/* TODO: write script to generate all of these instantiations for us? */

%template(VectorString)       std::vector<std::string>;
%template(LatLonCorners)      six::Corners<scene::LatLon>;
%template(LatLonAltCorners)   six::Corners<scene::LatLonAlt>;
%template(VectorMatchCollect) std::vector<six::MatchCollect>;
%template(VectorMatchType)    std::vector<six::MatchType>;

SCOPED_COPYABLE(six, Radiometric)
SCOPED_COPYABLE(six, ErrorStatistics)
SCOPED_COPYABLE(six, CorrCoefs)
SCOPED_COPYABLE(six, PosVelError)
SCOPED_COPYABLE(six, RadarSensor)
SCOPED_COPYABLE(six, TropoError)
SCOPED_COPYABLE(six, IonoError)
SCOPED_COPYABLE(six, CompositeSCP)
SCOPED_COPYABLE(six, Components)
SCOPED_COPYABLE(six, MatchInformation)
SCOPED_CLONEABLE(six, AmplitudeTable)

%extend mem::ScopedCloneablePtr<six::AmplitudeTable>
{
    //$self is a raw pointer to a ScopedCloneable container the
    //AmplitudeTable
    double __getitem__(size_t key) const
    {
        return *(double*)(**$self)[key];
    }

    void __setitem__(size_t key, double value)
    {
        double* location = (double*)(**$self)[key];
        *location = value;
    }
}
