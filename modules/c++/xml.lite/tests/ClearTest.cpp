/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <import/xml/lite.h>
#include <import/io.h>
#include <vector>
#include <iostream>
const char
        * xmldata =
                "<root okay=\"1\"><A>This</A><A>is</A><A>an</A><A><B>ugly</B></A><B>otter</B></root>";
void printCD(std::string tag, std::vector<xml::lite::Element*>& e)
{
    std::cout << tag << ": ";
    for (size_t i = 0; i < e.size(); ++i)
    {
        std::cout << e[i]->getCharacterData();
        std::cout << " ";
    }
    std::cout << std::endl;
}

int main()
{
    io::StringStream ss;
    ss.write(xmldata, strlen(xmldata));
    xml::lite::MinidomParser p;
    p.parse(ss);
    xml::lite::Element* elem = p.getDocument()->getRootElement();

    std::vector<xml::lite::Element*> vec;
    elem->getElementsByTagName("A", vec);
    printCD("A", vec);
    vec.clear();

    elem->getElementsByTagName("B", vec);
    printCD("B", vec);
    vec.clear();

    elem->getElementsByTagName("B", vec, true);
    printCD("B", vec);
    vec.clear();

    xml::lite::AttributeNode attr;
    attr.setQName("better");
    attr.setValue("1");
    xml::lite::Attributes atts;
    atts.add(attr);
    elem->setAttributes(atts);

    io::StandardOutStream sout;
    elem->print(sout);
    std::cout << std::endl;
}

