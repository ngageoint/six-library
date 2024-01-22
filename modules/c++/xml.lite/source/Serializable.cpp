/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#include "xml/lite/Serializable.h"

void xml::lite::Serializable::serialize(io::OutputStream& os)
{
    xml::lite::Element *root = getDocument()->getRootElement();
    if (root != nullptr)
    {
        os.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        root->print(os);
    }
}

void xml::lite::Serializable::deserialize(io::InputStream& is)
{
    //EVAL(is.available());
    mParser.parse(is);
}

