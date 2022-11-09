/*
 * =========================================================================
 * This file is part of except-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * except-python is free software; you can redistribute it and/or modify
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

%module(package="coda") coda_except

%feature("autodoc","1");

%include "config.i"

%{
  #include "import/except.h"
  using namespace except;
%}

%include "except/Context.h"
%include "except/Throwable.h"
%include "except/Exception.h"

%exception
{
    try
    {
        $action
    }
    catch (const std::exception& e)
    {
        if (!PyErr_Occurred())
        {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    }
    catch (const except::Exception& e)
    {
        if (!PyErr_Occurred())
        {
            PyErr_SetString(PyExc_RuntimeError, e.getMessage().c_str());
        }
    }
    catch (...)
    {
        if (!PyErr_Occurred())
        {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error");
        }
    }
    if (PyErr_Occurred())
    {
        SWIG_fail;
    }
}

%allowexception;
