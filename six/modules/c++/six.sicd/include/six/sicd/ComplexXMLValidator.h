/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SICD_COMPLEX_XML_VALIDATOR_H__
#define __SIX_SICD_COMPLEX_XML_VALIDATOR_H__

#include <memory>

#include <xml/lite/Document.h>
#include <six/XMLParser.h>
#include <six/SICommonXMLParser.h>
#include <six/sicd/ComplexData.h>

namespace six
{
namespace sicd
{
class ComplexXMLValidator
{
private:
    ComplexData mData;
};
}
}
#endif