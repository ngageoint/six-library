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

#include <import/nitf.h>
#include <import/except.h>
#include <import/sys.h>
#include "nitf/HandleManager.hpp"
#include "nitf/Field.hpp"
#include "nitf/BandInfo.hpp"
#include "nitf/SubWindow.hpp"
#include "nitf/List.hpp"
#include "nitf/FileHeader.hpp"
#include "nitf/HashTable.hpp"
#include "nitf/Extensions.hpp"
#include "nitf/TRE.hpp"
#include "nitf/ImageSegment.hpp"
#include "nitf/ImageSubheader.hpp"
#include "nitf/TextSegment.hpp"
#include "nitf/GraphicSegment.hpp"
#include "nitf/LabelSegment.hpp"
#include "nitf/DESegment.hpp"
#include "nitf/RESegment.hpp"
#include "nitf/Reader.hpp"
#include "nitf/Writer.hpp"
#include "nitf/PluginRegistry.hpp"
#include <iostream>
#include <vector>

template <typename T>
class Foo
{
protected:
    virtual void bar() {std::cout << "HERE1" << std::endl;}
public:
    Foo(){}
    virtual ~Foo(){bar();}
};


template <typename T>
class Bar : public Foo<T>
{
protected:
    virtual void bar() { std::cout << "HERE2" << std::endl; }
public:
    Bar(){}
    ~Bar(){bar();}
};


int main(int argc, char** argv)
{
    try
    {
        nitf_Error e;

//        Foo<int>* foo = new Bar<int>();
//        delete foo;

        nitf_Field* cField = nitf_Field_construct(100, NITF_BCS_N, &e);
        nitf::Field field(cField);

        {
            nitf::Field field2(cField);
        }

        std::cout << field.isValid() << std::endl;


        nitf_BandInfo* cBandInfo = nitf_BandInfo_construct(&e);
        assert(cBandInfo);

        nitf::BandInfo info(cBandInfo);

        std::cout << (int)info.getNumLUTs() << std::endl;

        std::cout << "HERE!!!!!!!!!!!!!!!!!!!!" << std::endl;
        nitf::SubWindow sub;
        nitf::SubWindow sub2 = sub;
        sub.setNumRows(5);

        nitf::PixelSkip p(1, 1);
        sub.setDownSampler(p);
        sub.setDownSampler(p);
        sub.setDownSampler(p);
        sub.setDownSampler(p);

        nitf::PixelSkip p2(1, 1);
        sub.setDownSampler(p2);
        sub.setDownSampler(p);


        nitf_SubWindow* subw = nitf_SubWindow_construct(&e);
        nitf::SubWindow sub3(subw);
        std::cout << sub.getNumRows() <<  " == " << sub2.getNumRows() << std::endl;


        nitf::List list;
        nitf::List list2 = list;

        {
            nitf::FileHeader header;
            nitf::FileHeader header2(header.clone());
            //should be not equal
            std::cout << "Equal? " << (header == header2) << std::endl;
            nitf::FileHeader header3(header2);
            //these two should be equal
            std::cout << "Equal? " << (header3 == header2) << std::endl;
        }

        nitf::HashTable hash;

        nitf::Extensions extensions;
        {
            nitf::ImageSegment imageSeg;
            nitf::ImageSubheader imageSub;

            imageSeg.getSubheader().getImageId() = "Test Image";
            std::cout << imageSeg.getSubheader().getImageId().toString() << std::endl;
            nitf::Field f = imageSeg.getSubheader().getImageId();
            std::cout << f.toString() << std::endl;

            nitf::ImageSegment imageSeg2 = imageSeg.clone();
            nitf::ImageSubheader imageSub2(imageSub.clone());

            extensions = imageSub.getExtendedSection();
        }

        nitf::TextSegment tSeg;
        nitf::TextSubheader tSub;

        nitf::GraphicSegment gSeg;
        nitf::GraphicSubheader gSub;

        nitf::DESegment dSeg;
        nitf::DESubheader dSub;

        nitf::LabelSegment rSeg;
        nitf::LabelSubheader rSub;

        nitf::TRE tre("JITCID");
        //tre.print();
        std::cout << "HERE!!!!!" << std::endl;

        nitf::Reader reader;
        nitf::Writer writer;


        //open a file
        sys::OS os;
        std::vector< std::string > files;
        for (int i = 1; i < argc; ++i)
        {
            if (!os.exists(argv[i]))
                std::cout << "Error -> File does not exist: " << argv[i] << std::endl;
            else
                files.push_back(argv[i]);
        }

        for (std::vector< std::string >::iterator it = files.begin(); it != files.end(); ++it)
        {
            nitf::IOHandle handle(*it);
            nitf::Reader rdr;
            nitf::Record rec = rdr.read(handle);

            std::cout << "CODEWORDS: " << rec.getHeader().getSecurityGroup().getCodewords().toString() << std::endl;
            rec.getHeader().getSecurityGroup().getCodewords() = "TEST";
            std::cout << "CODEWORDS: " << rec.getHeader().getSecurityGroup().getCodewords().toString() << std::endl;
            nitf::FileSecurity security;
            rec.getHeader().setSecurityGroup(security);
            std::cout << "CODEWORDS: " << rec.getHeader().getSecurityGroup().getCodewords().toString() << std::endl;
            std::cout << "Num Images: " << rec.getImages().getSize() << std::endl;
        }

        nitf_SubWindow_destruct(&subw);
        nitf_Field_destruct(&cField);
        nitf_BandInfo_destruct(&cBandInfo);
    }
    catch(except::Exception& ex)
    {
        std::cerr << "ERROR: " << ex.getMessage() << std::endl;
        return 1;
    }

    return 0;
}
