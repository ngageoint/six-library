/*
 * =========================================================================
 * This file is part of scene-python 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * scene-python is free software; you can redistribute it and/or modify
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
 *
 */

%module(package="pysix") scene

%feature("autodoc", "1");

%include "std_vector.i"
%include "std_string.i"

%import "except.i"
%import "math_linear.i"
%import "math_poly.i"
%import "types.i"

%{
  #include <cstddef>
  #include "import/except.h"
  #include "import/math/linear.h"
  #include "import/math/poly.h"
  #include "scene/Types.h"
  #include "scene/Utilities.h"
  #include "scene/CoordinateTransform.h"
  #include "scene/ECEFToLLATransform.h"
  #include "scene/LLAToECEFTransform.h"
  #include "scene/LocalCoordinateTransform.h"
  #include "scene/SceneGeometry.h"
  #include "scene/ProjectionModel.h"
  #include "scene/AdjustableParams.h"
  #include "scene/Errors.h"
  #include "scene/FrameType.h"
  #include "scene/GridGeometry.h"
%} 

%rename("SceneUtilities") Scene::Utilities;

%include "scene/Types.h"
%include "scene/EllipsoidModel.h"
%include "scene/Utilities.h"
%include "scene/CoordinateTransform.h"
%include "scene/ECEFToLLATransform.h"
%include "scene/LLAToECEFTransform.h"
%include "scene/LocalCoordinateTransform.h"
%include "scene/SceneGeometry.h"
%include "scene/FrameType.h"
%include "scene/Errors.h"
%include "scene/AdjustableParams.h"
%include "scene/ProjectionModel.h"
%include "scene/GridGeometry.h"



%extend scene::AdjustableParams 
{
  public:
    double __getitem__(std::ptrdiff_t idx) 
    {
      if(idx >= scene::AdjustableParams::NUM_PARAMS || idx < 0) 
      {
        return 0.0;
      }
      return $self->mParams[idx];
    }
    void __setitem__(std::ptrdiff_t idx, double val) 
    {
      if(idx >= scene::AdjustableParams::NUM_PARAMS || idx < 0) 
      {
        return;
      }
      $self->mParams[idx] = val;
    }
};


%extend scene::ProjectionModel
{
  /* Extend the Python interface to imageToScene to take a sequence of  */
  /* row indices and a sequence of column indices and return a list of  */
  /* projected points                                                   */
  PyObject* imageToScene(PyObject* row_seq,
                         PyObject* col_seq,
                         math::linear::VectorN<3,double> ground_ref_point,
                         math::linear::VectorN<3,double> ground_plane_normal)
  {
    if (!PySequence_Check(row_seq))
    {
      PyErr_SetString(PyExc_TypeError,"Expecting a sequence of row indices.");
      return NULL;
    }
    if (!PySequence_Check(col_seq))
    {
      PyErr_SetString(PyExc_TypeError,"Expecting a sequence of column indices.");
      return NULL;
    }
    Py_ssize_t N = PyObject_Length(row_seq);
    if (N != PyObject_Length(col_seq))
    {
      PyErr_SetString(PyExc_ValueError,"Input row and column sequences must have same length.");
      return NULL;
    }
    math::linear::VectorN<3,double>* vec_ptr;
    PyObject* pyresult = PyList_New(N);
    PyObject* pytmp;
    for (Py_ssize_t i = 0; i < N; ++i)
    {
      PyObject* o1 = PySequence_GetItem(row_seq, i);
      PyObject* o2 = PySequence_GetItem(col_seq, i);
      types::RowCol<double> rc(PyFloat_AsDouble(o1), PyFloat_AsDouble(o2));
      math::linear::VectorN<3,double> tmp = (*self).imageToScene(rc, 
                                                                 ground_ref_point, 
                                                                 ground_plane_normal);
      vec_ptr = new math::linear::VectorN<3,double>(tmp);
      pytmp = SWIG_NewPointerObj(SWIG_as_voidptr(vec_ptr),
                                 SWIGTYPE_p_math__linear__VectorNT_3_double_t,
                                 0 | 0);
      PyList_SetItem(pyresult, i, pytmp);
    }
    return pyresult;
  }
}