/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six-c++ is free software; you can redistribute it and/or modify
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

#include <cstdio>
#include <fstream>
#include <six/NITFWriteControl.h>
#include <six/Types.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>
#include <six/sidd/DerivedXMLControl.h>

namespace
{
    void validateArguments(int argc, char** argv)
    {
        if (argc != 3)
        {
            std::string message = "Usage: " + sys::Path::basename(argv[0])
                + " <SICD pathname> <output SICD pathname>";
            throw except::Exception(Ctxt(message));
        }
        if (!str::endsWith(argv[1], ".nitf") && !str::endsWith(argv[1], ".ntf"))
        {
            std::string message = "Extension suggests input may not be a SICD.";
            throw except::Exception(Ctxt(message));
        }
    }

    std::vector<six::UByte> generateBandData(const six::Data& data)
    {
        std::vector<six::UByte> bandData(data.getNumRows() * data.getNumCols()
            * data.getNumBytesPerPixel());

        for (size_t ii = 0; ii < bandData.size(); ++ii)
        {
            bandData[ii] = static_cast<six::UByte>(ii);
        }

        return bandData;
    }

    six::DataType getDataType(const std::string& pathname)
    {
        six::XMLControlFactory::getInstance().addCreator(
            six::DataType::COMPLEX,
            new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());
        six::XMLControlFactory::getInstance().addCreator(
            six::DataType::DERIVED,
            new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>());

        six::NITFReadControl reader;
        reader.load(pathname);
        six::Container* const container = reader.getContainer();
        six::Data* const data = container->getData(0);
        return data->getDataType();
    }

    void attachDESs(const std::string& originalPathname,
        const std::string& outputPathname)
    {
        six::XMLControlFactory::getInstance().addCreator(
            six::DataType::COMPLEX,
            new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());
        six::XMLControlFactory::getInstance().addCreator(
            six::DataType::DERIVED,
            new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>());

        six::NITFReadControl reader;
        reader.load(originalPathname);
        six::Container* const container = reader.getContainer();

        six::NITFWriteControl writer;
        writer.initialize(container);

        std::vector<six::UByte> bandData(
            generateBandData(*container->getData(0)));

        nitf::Record record = writer.getRecord();
        nitf::DESegment des = record.newDataExtensionSegment();
        des.getSubheader().getFilePartType().set("DE");
        des.getSubheader().getTypeID().set("XML_DATA_CONTENT");
        des.getSubheader().getVersion().set("01");
        des.getSubheader().getSecurityClass().set("U");

        static const char segmentData[] = "123456789ABCDEF0";
        nitf::SegmentMemorySource sSource(segmentData, strlen(segmentData),
            0, 0, true);
        mem::SharedPtr<nitf::SegmentWriter> segmentWriter(new nitf::SegmentWriter);
        segmentWriter->attachSource(sSource);
        writer.addAdditionalDES(segmentWriter);

        //Wrong tag
        //TODO: Figure out why this breaks things
        //nitf::TRE usrHdr("PIAIMB", "PIAIMB");
        //record.getHeader().getUserDefinedSection().appendTRE(usrHdr);

        //Good tag, wrong DESSHSI
        nitf::TRE secondHdr(six::Constants::DES_USER_DEFINED_SUBHEADER_TAG,
            six::Constants::DES_USER_DEFINED_SUBHEADER_ID);
        secondHdr.setField("DESCRC", "99999");
        secondHdr.setField("DESSHFT", "XML00000");
        secondHdr.setField("DESSHDT", "2016-06-01T00:00:00Z");
        secondHdr.setField("DESSHRP", "1234567890123456789012345678901234567890");
        secondHdr.setField("DESSHSI", std::string(" ", 60));
        secondHdr.setField("DESSHSV", "Version 10");
        secondHdr.setField("DESSHSD", "2016-06-01T00:00:00Z");
        secondHdr.setField("DESSHTN", std::string(" ", 120));
        secondHdr.setField("DESSHLPG", std::string("1", 125));
        secondHdr.setField("DESSHLPT", std::string(" ", 20));
        secondHdr.setField("DESSHLI", std::string(" ", 20));
        secondHdr.setField("DESSHLIN", std::string(" ", 120));
        secondHdr.setField("DESSHABS", std::string(" ", 200));
        des.getSubheader().setSubheaderFields(secondHdr);

        //Wrong length
        nitf::TRE middleTRE(six::Constants::DES_USER_DEFINED_SUBHEADER_TAG,
            "XML_DATA_CONTENT_283");
        middleTRE.setField("DESCRC", "99999");
        middleTRE.setField("DESSHFT", "XML00000");
        middleTRE.setField("DESSHDT", "2016-06-01T00:00:00Z");
        middleTRE.setField("DESSHRP", "1234567890123456789012345678901234567890");
        middleTRE.setField("DESSHSI", std::string(" ", 60));
        middleTRE.setField("DESSHSV", "Version 10");
        middleTRE.setField("DESSHSD", "2016-06-01T00:00:00Z");
        middleTRE.setField("DESSHTN", std::string(" ", 120));

        nitf::DESegment middleDES = record.newDataExtensionSegment();
        middleDES.getSubheader().getFilePartType().set("DE");
        middleDES.getSubheader().getTypeID().set("XML_DATA_CONTENT");
        middleDES.getSubheader().getVersion().set("01");
        middleDES.getSubheader().getSecurityClass().set("U");

        nitf::SegmentMemorySource middleSource(segmentData, strlen(segmentData),
            0, 0, true);
        mem::SharedPtr<nitf::SegmentWriter> middleSegmentWriter(new nitf::SegmentWriter);
        middleSegmentWriter->attachSource(middleSource);
        writer.addAdditionalDES(middleSegmentWriter);

        //Wrong length again
        nitf::TRE shortTRE(six::Constants::DES_USER_DEFINED_SUBHEADER_TAG,
            "XML_DATA_CONTENT_005");
        shortTRE.setField("DESCRC", "99999");

        nitf::DESegment shortDES = record.newDataExtensionSegment();
        shortDES.getSubheader().getFilePartType().set("DE");
        shortDES.getSubheader().getTypeID().set("XML_DATA_CONTENT");
        shortDES.getSubheader().getVersion().set("01");
        shortDES.getSubheader().getSecurityClass().set("U");
        nitf::SegmentMemorySource shortSource(segmentData, strlen(segmentData),
            0, 0, true);
        mem::SharedPtr<nitf::SegmentWriter> shortSegmentWriter(
            new nitf::SegmentWriter);
        shortSegmentWriter->attachSource(shortSource);
        writer.addAdditionalDES(shortSegmentWriter);
        writer.save(&bandData[0], outputPathname);
    }

    class TestScript
    {
    public:
        size_t findSourceLine() const
        {
            size_t index = 0;
            for (index = 1; index < mLines.size(); ++index)
            {
                if (!mLines[index].empty())
                {
                    break;
                }
            }

            if (index == 0)
            {
                throw except::Exception(Ctxt("Test script appears empty"));
            }

            return index;
        }

        TestScript(const std::string& pathname) :
            mPathname(pathname)
        {
            std::ifstream file(mPathname.c_str());
            std::string line;
            while (std::getline(file, line))
            {
                mLines.push_back(line);
            }

            std::string sourceLine = mLines[findSourceLine()];
            mOriginalSource = str::split(sourceLine, " ")[1];
        }

        ~TestScript()
        {
            setSource(mOriginalSource);
        }

        void setSource(const std::string& sourcePathname)
        {
            std::vector<std::string> sourceLine = str::split(
                    mLines[findSourceLine()], " ");
            sourceLine[1] = sourcePathname;
            mLines[findSourceLine()] = str::join(sourceLine, " ");
            write();
        }

    private:
        std::vector<std::string> mLines;
        std::string mOriginalSource;
        const std::string mPathname;

        void write()
        {
            std::ofstream file(mPathname.c_str());
            for (size_t ii = 0; ii < mLines.size(); ++ii)
            {
                file << mLines[ii] << "\n";
            }
        }
    };

    class ResultFile
    {
    public:
        ResultFile();
        ~ResultFile();
        std::string pathname() const;
        bool operator==(const ResultFile& rhs) const;
    private:
        std::string mName;
    };

    ResultFile::ResultFile() :
        mName(std::tmpnam(NULL))
    {
    }

    ResultFile::~ResultFile()
    {
        std::remove(mName.c_str());
    }

    std::string ResultFile::pathname() const
    {
        return mName;
    }

    bool ResultFile::operator==(const ResultFile& rhs) const
    {
        std::string leftLine;
        std::string rightLine;
        std::ifstream leftFile(pathname().c_str());
        std::ifstream rightFile(rhs.pathname().c_str());

        while (std::getline(leftFile, leftLine) &&
            std::getline(rightFile, rightLine))
        {
            str::trim(leftLine);
            str::trim(rightLine);
            if (str::startsWith(leftLine, "Program End Time"))
            {
                break;
            }

            if (str::startsWith(leftLine, "Program Start Time") ||
                str::endsWith(leftLine, ".nitf"))
            {
                continue;
            }

            if (leftLine != rightLine)
            {
                return false;
            }
        }

        return true;
    }
}

int main(int argc, char** argv)
{
    try
    {
        std::cerr << "VTS.exe\n";
        validateArguments(argc, argv);
        const std::string inputPathname(argv[1]);
        const std::string outputPathname(argv[2]);

        attachDESs(inputPathname, outputPathname);
        std::cerr << "DESs attached\n";

        const std::string delimiter(sys::Path::delimiter());
        const std::string testName =
                (getDataType(inputPathname) == six::DataType::COMPLEX) ?
                "test_script_sicd" : "test_script_sidd";
        const std::string testFile = sys::Path::joinPaths("six",
            sys::Path::joinPaths("projects",
                sys::Path::joinPaths("csm",
                    sys::Path::joinPaths("tests", testName))));
        TestScript testScript(testFile);
        testScript.setSource(inputPathname);
        std::string vts = sys::Path::joinPaths("install",
            sys::Path::joinPaths("bin", "vts"));
        sys::OS os;
        std::cerr << "OS name: " << os.getPlatformName() << std::endl;
        if (os.isFile(vts + ".exe"))
        {
            vts = vts + ".exe";
        }

        ResultFile originalResult;
        sys::Exec originalCommand(vts + " < " + testFile + " > "
            + originalResult.pathname());

        originalCommand.run();

        testScript.setSource(outputPathname);
        ResultFile roundTrippedResult;
        sys::Exec roundTrippedCommand(vts + " < " + testFile + " > "
            + roundTrippedResult.pathname());
        roundTrippedCommand.run();

        if (originalResult == roundTrippedResult)
        {
            std::cout << "Test passed" << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "VTS output does not match. Test failed" << std::endl;
            return 1;
        }
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
