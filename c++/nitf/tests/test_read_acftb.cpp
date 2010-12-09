/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.hpp>
#include <import/except.h>

//struct TREPrintFunctor : public nitf::TRETraverser
//{
//    void operator()(nitf::TRE* tre, nitf::TREDescription* desc,
//                    const std::string& tag, nitf::Field field, NITF_DATA* userData)
//    {
//        std::cout << (desc->label ? desc->label : "null") <<
//        "(" << tag << ") = [" << field.toString() << "]" << std::endl;
//    }
//};


void findInExtensions(nitf::Extensions ext)
{
    try
    {
        //TREPrintFunctor treTraverser;
        nitf::List l = ext.getTREsByName("ACFTB");
        nitf::ListIterator iter;
        nitf::ListIterator end = l.end();
        for (iter = l.begin(); iter != end; ++iter )
        {

            nitf::TRE tre = *iter;
            //tre.foreach(treTraverser);
            if (tre.exists( "raw_data" ) )
            {
                std::cout
                << "Your plugin for ACFTB was not loaded so the data is contained in the RAW section" << std::endl;
            }
            else
            {
                try
                {
                    // This will throw a NoSuchKeyException if
                    // it is not found
                    nitf::Field v = tre.getField( "ACMSNID" );
                    std::cout << "Mission ID: [" << v.toString() << "]" << std::endl;

                }
                catch (except::NoSuchKeyException& none)
                {
                    throw nitf::NITFException(none,
                                              Ctxt("Error: no Mission ID available"));

                }
            }
        }
    }
    catch (except::NoSuchKeyException& noACFTB)
    {
        std::cout << "No ACFTB" << std::endl;
        return ;
    }

}


int main(int argc, char** argv)
{
    try
    {
        nitf::Reader reader;
        if (argc != 2)
        {
            throw nitf::NITFException(Ctxt(FmtX("Usage: %s <nitf-file>\n",
                                                argv[0])));
        }
        nitf::IOHandle io(argv[1]);
        nitf::Record record = reader.read(io);

        nitf::ListIterator end = record.getImages().end();

        for (nitf::ListIterator iter = record.getImages().begin();
                iter != end; ++iter)
        {
            nitf::ImageSegment imageSegment = *iter;
            assert( imageSegment.isValid() );

            if ( imageSegment.getSubheader().getUserDefinedSection().isValid() )
            {
                std::cout << "ONE" << std::endl;
                findInExtensions( imageSegment.getSubheader().getUserDefinedSection() );
            }
            else
                std::cout << "Nothing found in user defined section!"
                << std::endl;

            if ( imageSegment.getSubheader().getExtendedSection().isValid() )
            {
                std::cout << "TWO" << std::endl;
                findInExtensions( imageSegment.getSubheader().getExtendedSection());
            }
            else
                std::cout << "Nothing found in extended section!" << std::endl;

        }

    }
    catch (except::Throwable& t)
    {
        std::cout << t.getMessage() << std::endl;
        std::cout << t.getTrace() << std::endl;
    }
    return 0;
}

