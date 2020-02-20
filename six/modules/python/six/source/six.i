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

%include <std_vector.i>
%include <std_string.i>
%include <std_auto_ptr.i>

%{

#include <memory>
#include <vector>
#include <cstddef>
using std::ptrdiff_t;

#include "import/nitf.hpp"
#include "import/six.h"
#include "Python.h"
#include "datetime.h"

using namespace six;
%}

%init %{
    PyDateTime_IMPORT;
%}

// This allows functions that deal with auto_ptrs to work properly
%auto_ptr(six::Data);
%auto_ptr(six::XMLControlCreator);

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
%include "six/CollectionInformation.h"
%include "six/Classification.h"
%include "six/ErrorStatistics.h"
%include "six/MatchInformation.h"
%include "six/Radiometric.h"
%include "six/Data.h"
%include "six/XMLControl.h"
%include "six/Utilities.h"
%include "six/Options.h"
%include "six/XMLControlFactory.h"

%feature("shadow") six::Parameter::setValue(const std::string &)
%{
def setValue(self, *args):
    if len(args) != 1:
        raise RuntimeError("Parameter.setValue takes exactly one argument")
    new_tuple = (str(args[0]), )
    return $action(self, str(args[0]))
%}


%extend nitf::DateTime
{
  public:
    PyObject* toPythonDateTime()
    {
        PyObject* nitfDateTime = NULL;
        int year = self->getYear();
        int month = self->getMonth();
        int day = self->getDayOfMonth();
        int hour = self->getHour();
        int minute = self->getMinute();
        int second = static_cast<int>(self->getSecond());
        int microsecond = static_cast<int>((self->getSecond() - second) * 1e6);
        nitfDateTime = PyDateTime_FromDateAndTime(year, month, day,
                hour, minute, second, microsecond);
        return nitfDateTime;
    }

    %pythoncode
    %{
        @staticmethod
        def fromPythonDateTime(pyDatetime):
            second = pyDatetime.second + (pyDatetime.microsecond / 1e6);
            sixDatetime = DateTime(pyDatetime.year,
                    pyDatetime.month,
                    pyDatetime.day,
                    pyDatetime.hour,
                    pyDatetime.minute,
                    second)
            return sixDatetime
    %}
}

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

%template(LatLonCorners) six::Corners<scene::LatLon>;
%template(LatLonAltCorners) six::Corners<scene::LatLonAlt>;

SCOPED_COPYABLE(six, Radiometric)
SCOPED_COPYABLE(six, ErrorStatistics)
SCOPED_COPYABLE(six, MatchInformation)
SCOPED_COPYABLE(six, CorrCoefs)
SCOPED_COPYABLE(six, PosVelError)
SCOPED_COPYABLE(six, RadarSensor)
SCOPED_COPYABLE(six, TropoError)
SCOPED_COPYABLE(six, IonoError)
SCOPED_COPYABLE(six, CompositeSCP)
SCOPED_COPYABLE(six, Components)
SCOPED_CLONEABLE(six, AmplitudeTable)
SCOPED_CLONEABLE(six, CollectionInformation)

SCOPED_COPYABLE(six, MatchType)

%template(VectorMatchCollect) std::vector<six::MatchCollect>;
%template(VectorScopedCopyableMatchType) std::vector<mem::ScopedCopyablePtr<six::MatchType> >;

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
