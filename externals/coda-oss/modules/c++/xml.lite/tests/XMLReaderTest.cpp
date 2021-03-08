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

/*!
 *  \file XMLEventReaderTest.cpp
 *  \brief Test to prove the XMLEventReader worth
 *
 *  This will first stream the input file to a temp file.  Then it will attempt
 *  to parse it with the Reporter class, a direct descendant of the
 *  XMLEventReader.  For an interesting test, run it with the argument of
 *  make.xml (if it is in this folder).
 *
 */
#if defined(USE_EXPAT) || defined(USE_XERCES) || defined(USE_LIBXML)

#include <import/io.h>
#include <import/sys.h>
#include <import/except.h>
#include <import/xml/lite.h>

using namespace std;
using namespace io;
using namespace sys;
using namespace except;
using namespace xml::lite;

class Reporter : public xml::lite::ContentHandler
{
public:
    Reporter() : mDepth(0)
    {}
    ~Reporter()
    {}

    void printAttributes(const xml::lite::Attributes& attr)
    {
        for (int i = 0; i < attr.getLength(); i++)
        {
            cout << "Attribute " << i << ": " << endl;
            cout << "Uri: " << attr.getUri(i) << endl;
            cout << "Local: " << attr.getLocalName(i) << endl;
            cout << "QName: " << attr.getUri(i) << endl;
            cout << "Value: " << attr.getValue(i) << endl;
        }
    }

    /*!
     *  Define char data method.  Fired when character data is found.
     *  \param  data   The data picked up by the handler
     *  \param  length The length of the data picked up by the handler
     */
    void characters(const char *data, int length)
    {
        // Do nothing: Im not interested in keeping track of this
    }

    /*!
     *  Define  start element method.  Fired when a begin tag is found.
     *  \param  name    The name of the tag
     *  \param  atts A list of attributes in the begin tag 
     */
    void startElement(const std::string & uri,
                      const std::string & localName,
                      const std::string & qname,
                      const xml::lite::Attributes & atts)
    {
        printDepth();
        cout << "START ELEMENT "
        << "(uri=\"" << uri << "\" "
        << "localName=\"" << localName << "\" "
        << "qname=\"" << qname << "\")" << endl;
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        printDepth();
        cout << "<== BEGIN ATT LIST ==> " << endl;
        printAttributes(atts);
        printDepth();
        cout << "<== END ATT LIST ==> " << endl;
        ++mDepth;
        cout << "================================================" << endl;
    }

    /*!
     *  Define  end element method.  Fired when a begin tag is found.
     *  \param  name   The name of the element tag
     */
    void endElement(const std::string & uri,
                    const std::string & localName,
                    const std::string & qname)
    {
        printDepth();
        cout << "END ELEMENT "
        << "(uri=\"" << uri << "\" "
        << "localName=\"" << localName << "\" "
        << "qname=\"" << qname << "\")" << endl;
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        cout << "================================================" << endl;
        --mDepth;
    }

private:

    void printDepth()
    {
        for (int i = 0; i < mDepth; i++)
            cout << ' ';
    }
    int mDepth;
};

// If I put in a streamTo() to an output stream, it works
// Otherwise it runs forever
int main(int argc, char **argv)
{
    try
    {
        // Check to make sure we have right length
        if (argc != 2)
            throw Exception(Ctxt(FmtX("Usage: %s <xml file>\n", argv[0])));

        // Create an input stream
        FileInputStream
        xmlFile(argv[1]);

        xml::lite::XMLReader xmlReader;
        
        std::cout << "XML Driver: " << xmlReader.getDriverName() << std::endl;
        xmlReader.setContentHandler(new Reporter());

        //EVAL( s.stream().str() );
        // Use the provided parse method to parse the input file
        //xmlReporter.parse(s);
        xmlReader.parse(xmlFile);
        xmlFile.close();
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

