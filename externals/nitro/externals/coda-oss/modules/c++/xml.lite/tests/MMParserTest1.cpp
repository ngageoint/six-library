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

#if defined (USE_EXPAT) || defined(USE_XERCES) || defined(USE_LIBXML)

#include <import/io.h>
#include <import/xml/lite.h>
#include <import/except.h>
//#include <import/util.h>
using namespace std;
using namespace io;
using namespace xml::lite;
using namespace except;

int main(int argc, char **argv)
{
    try
    {

        // Check to make sure we have right length
        if (argc != 3)
            die_printf("Usage: %s --map|--file <xml file>\n", argv[0]);

        InputStream* xmlFile;
        // Create an input stream
        std::string flags = argv[1];
        if (flags == "--file")
            xmlFile = new FileInputStream(argv[2]);
        else if (flags == "--map")
            xmlFile = new MMapInputStream(argv[2]);
        else
            die_printf("Invalid flag: [%s]\n", flags.c_str());



        // Create an XML parse tree
        MinidomParser treeBuilder;
//#if defined(USE_XERCES)
//      treeBuilder.getReader().setValidation(true);
//#endif
        // Use the provided parse method to parse the input file
        treeBuilder.parse(*xmlFile);


        std::cout << "Finished!" << std::endl;
        /*
        // Get the top level node so we can manipulate the tree
        Element *rootNode = treeBuilder.getDocument()->getRootElement();

        //assert(rootNode2 == rootNode);
        //      cout << "========== Begin XML Tree ==========" << endl;
        StandardOutStream out;

        rootNode->print(out);
        //      cout << "========== End  XML  Tree ==========" << endl;

        */
        // The Tree should delete itself
        delete xmlFile;

    }
    // Catch all throwables and exit in a reasonable manner
    catch (Throwable & t)
    {
        cout << "Caught Throwable: " << t.toString() << endl;

        // If we caught an exception, exit unfavorably.
        exit(EXIT_FAILURE);
    }
    return 0;
}
#else
int main()
{}
#endif

