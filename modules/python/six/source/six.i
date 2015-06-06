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

%module six_base

%feature("autodoc","1");

%include "std_vector.i"
%include "std_string.i"

%{

#include <memory>
#include <cstddef>
using std::ptrdiff_t;

#include "six/Utilities.h"
#include "import/six.h"

/* this isn't imported by the above include */
#include "six/Radiometric.h"

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


%import "except.i"
%import "math_poly.i"
%import "scene.i"

/* parametric elt-size array */
/* will probably want it eventually, but it looks like six.sicd doesn't use it */
%ignore "LUT";

/* auto_ptr causes problems, as well as 
 * xml factory stuff that we'll just 
 * put aside for now
 */
%ignore parseData; 

/* prevent name conflicts */
%rename("SixUtilities") six::Utilities;

/* six::Parameter does not play nicely with SWIG currently */
namespace six 
{
  %typemap(typecheck) (Parameter) 
  {
    if(!PyTuple_Check($input) || !(2 != PyTuple_Size($input))) 
    {
      $1 = 0; 
    } 
    else 
    {
      $1 = 1;
    }
  }

  %typemap(in) (Parameter) 
  {
    if(!PyTuple_Check($input)) 
    {
      PyErr_SetString(PyExc_ValueError, "Expected a two element string tuple");
      return NULL;
    }
    if(2 != PyTuple_Size($input)) 
    {
      PyErr_SetString(PyExc_ValueError, "Expected a two element string tuple");
      return NULL;
    }
    PyObject* pyName  = PyTuple_GetItem($input,0);
    PyObject* pyValue = PyTuple_GetItem($input,1);
    std::string name  = PyString_AsString(pyName);
    std::string value = PyString_AsString(pyValue);
    six::Parameter param;
    param.setName(name);
    param.setValue(value);
    $1 = param;
  }
 
  %typemap(out) (Parameter) 
  {
    PyObject* out_tuple = PyTuple_New(2);
    PyObject* pyName  = PyString_FromString( $1.getName().c_str() );
    PyObject* pyValue = PyString_FromString( $1.str().c_str() );
    PyTuple_SetItem(out_tuple, 0, pyName);
    PyTuple_SetItem(out_tuple, 1, pyValue);
    $result = out_tuple;
  }

}


/* current six python interface consists of these files */
%include "nitf/DateTime.hpp"
%include "six/Enums.h"
%include "six/Types.h"
%include "six/Init.h"
%include "six/Classification.h"
%include "six/ErrorStatistics.h"
%include "six/Radiometric.h"
%include "six/Data.h"
%include "six/XMLControl.h"
%include "six/Utilities.h"
%include "six/Options.h"

%template(VectorString) std::vector<std::string>;
%template(LatLonCorners) six::Corners<scene::LatLon>;

