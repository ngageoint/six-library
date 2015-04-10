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

%include "math.linear/source/math_linear.i" 

%{
  #include "scene/Types.h"
  #include "scene/Utilities.h"
  #include "scene/CoordinateTransform.h"
  #include "scene/ECEFToLLATransform.h"
  #include "scene/LLAToECEFTransform.h"
  #include "scene/LocalCoordinateTransform.h"
%}

%include "scene/Types.h"
%include "scene/EllipsoidModel.h"
%include "scene/Utilities.h"
%include "scene/CoordinateTransform.h"
%include "scene/ECEFToLLATransform.h"
%include "scene/LLAToECEFTransform.h"
%include "scene/LocalCoordinateTransform.h"

