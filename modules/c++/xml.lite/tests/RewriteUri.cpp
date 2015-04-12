/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#if defined(USE_EXPAT) || defined(USE_XERCES)

#include <import/except.h>
#include <import/io.h>
#include <import/xml/lite.h>

int main(int argc, char **argv)
{
    if (argc != 5)
        die_printf("Usage: %s <file> <prefix> <new-uri> <outfile>\n",
                   argv[0]);

    try
    {

        std::string file    = argv[1];
        std::string prefix  = argv[2];
        std::string uri     = argv[3];
        std::string outfile = argv[4];

        std::cout << "Replacing all matching prefixes " << prefix
            << " with uri " << uri << std::endl;
        io::FileInputStream fis( file );

        xml::lite::MinidomParser parser;
        parser.parse(fis);
        xml::lite::Element* topLevel = parser.getDocument()->getRootElement();
        topLevel->setNamespaceURI( prefix, uri );
        io::FileOutputStream fos( outfile );
        topLevel->prettyPrint( fos );
    }
    catch (except::Throwable& anything)
    {
        std::cout << "Caught throwable: " << anything.getType() << " "
        << anything.toString() << std::endl;

    }

}
#else
int main()
{}
#endif

