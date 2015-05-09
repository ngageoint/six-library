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

%module scene

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
  typedef math::linear::VectorN<3,double> Vector3;
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
