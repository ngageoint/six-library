/*
 * =========================================================================
 * This file is part of coda_logging-python 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * coda_logging-python is free software; you can redistribute it and/or modify
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

%module coda_logging

%feature("autodoc", "1");

%{
  #include "import/logging.h"

  using namespace logging;
%}

%include "logging/Filterer.h"
%include "logging/Handler.h"
%include "logging/Filter.h"
%include "logging/Logger.h"
%include "logging/NullLogger.h"

