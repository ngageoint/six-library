/*
 * =========================================================================
 * This file is part of xml.lite-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * xml.lite-python is free software; you can redistribute it and/or modify
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


%module(package="coda") xml_lite

%feature("autodoc", "1");

%include "config.i"

%{
  #include "import/xml/lite.h"
  using xml::lite::Element;
  using xml::lite::Document;
  using xml::lite::MinidomParser;
  using xml::lite::XMLReader;
  using xml::lite::MinidomHandler;
%}

%ignore xml::lite::Element::setAttributes;
%ignore xml::lite::Element::getAttributes;
%ignore xml::lite::Element::addChild;
%ignore xml::lite::Element::create; // returns unique_ptr: unsupported by SWIG
%ignore xml::lite::Element::createU8; // returns unique_ptr: unsupported by SWIG

%include "xml/lite/Element.h"
%include "xml/lite/Document.h"
%include "xml/lite/MinidomParser.h"

