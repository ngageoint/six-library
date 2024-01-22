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
#include <import/sys.h>
#include <import/xml/lite.h>
#include <import/except.h>
#include <string>

using namespace std;
using namespace sys;
using namespace io;
using namespace xml::lite;
using namespace except;

const std::string ROOT_TAG = "deploy";
const std::string ROOT_URI = "";
const std::string SERVICE_TAG = "service";
const std::string SERVICE_URI = "";
const std::string LOCATION_TAG = "location";
const std::string LOCATION_URI = "";
const std::string NAME_TAG = "name";
const std::string NAME_URI = "";

void addService(Document *doc, Element *rootElement, string name, string location);
bool removeService(Document *doc, Element *rootElement, string name);
bool findService(Document *doc, Element *rootElement, string name);

int main(int argc, char **argv)
{
    try
    {
        if (argc != 5)
        {
            throw Exception(Ctxt(str::Format(
                                     "Usage: %s [OPTION] <service> <uri> <file>\n\n\t-a\tadd service\n\t-s\tsubtract service\n\t-o\toutput file\n",
                                     argv[0])));
        }

        StandardOutStream out;

        // Create an XML parse tree
        MinidomParser treeBuilder;
#if defined(USE_XERCES)
        treeBuilder.getReader().setValidation(true);
#endif

        sys::OS os;

        if (!os.exists(argv[4]))
        {
            FileOutputStream os(argv[4]);
            os.write("<deploy>\n</deploy>");
            os.close();
        }
        FileInputStream inXmlFile(argv[4]);

        // Use the provided parse method to parse the input file
        treeBuilder.parse(inXmlFile);

        inXmlFile.close();

        Document *doc = treeBuilder.getDocument();
        Element *rootElement = doc->getRootElement();

        string name = argv[2];
        string location = argv[3];

        FileOutputStream outXmlFile;

        if (!rootElement)
        {
            rootElement = doc->createElement(ROOT_TAG, ROOT_URI, ROOT_TAG);
            doc->setRootElement(rootElement);
        }
        else if (rootElement->getLocalName() != ROOT_TAG)
        {
            throw Exception(Ctxt("Invalid deploy file"));
        }

        switch (argv[1][1])
        {
            case 'a':
                addService(doc, rootElement, name, location);
#if defined(USE_IO_STREAMS)
                outXmlFile.open(argv[4]);
#else
                outXmlFile.create(argv[4]);
#endif
                if (rootElement) rootElement->print(outXmlFile);
                break;
            case 's':
                removeService(doc, rootElement, name);
                rootElement = doc->getRootElement();
#if defined(USE_IO_STREAMS)
                outXmlFile.open(argv[4]);
#else
                outXmlFile.create(argv[4]);
#endif
                if (rootElement) rootElement->print(outXmlFile);
                break;
            case 'o':
                if (rootElement) rootElement->print(out);
                break;
            default:
                throw Exception(Ctxt(str::Format(
                                         "Usage: %s [OPTION] <service> <uri> <file>\n\n\t-a\tadd service\n\t-s\tsubtract service\n\t-o\toutput file\n",
                                         argv[0])));
        }

        outXmlFile.close();

    }    // Catch all throwables and exit in a reasonable manner
    catch (Throwable & t)
    {
        cout << t.toString() << endl;
        // If we caught an exception, exit unfavorably.
        exit(EXIT_FAILURE);
    }
    return 0;
}


void addService(Document *doc, Element *rootElement, string name, string location)
{
    Element *toAdd, *child;

    if (findService(doc, rootElement, name)) return;

    toAdd = doc->createElement(SERVICE_TAG, SERVICE_URI, "" );
    child = doc->createElement(NAME_TAG, NAME_URI, NAME_TAG);
    child->setCharacterData(name);
    toAdd->addChild(child);
    child = doc->createElement(LOCATION_TAG, LOCATION_URI, "");
    child->setCharacterData(location);
    toAdd->addChild(child);

    doc->insert(toAdd, rootElement);
}

bool removeService(Document *doc, Element *rootElement, string name)
{
    bool found(false);
    vector< Element* >::iterator childIterator;

    childIterator = rootElement->getChildren().begin();
    for (; childIterator != rootElement->getChildren().end(); ++childIterator)
    {
        if ((*childIterator)->getCharacterData() == name)
        {
            doc->remove(rootElement);
            return true;
        }
        else
        {
            found = removeService(doc, *childIterator, name);
            if (found) return true;
        }
    }
    return false;
}

bool findService(Document *doc, Element *rootElement, string name)
{
    bool found(false);
    vector< Element* >::iterator childIterator;

    childIterator = rootElement->getChildren().begin();
    for (; childIterator != rootElement->getChildren().end(); ++childIterator)
    {
        if ((*childIterator)->getCharacterData() == name)
        {
            return true;
        }
        else
        {
            found = findService(doc, *childIterator, name);
            if (found) return true;
        }
    }
    return false;
}
#else
int main()
{}
#endif

