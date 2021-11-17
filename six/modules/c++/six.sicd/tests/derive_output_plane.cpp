/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <algorithm>

#include <six/Container.h>
#include <six/NITFWriteControl.h>
#include <six/sicd/AreaPlaneUtility.h>
#include <six/sicd/ComplexData.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

namespace
{
void roundTripNITF(const std::string& sicdPathname,
        const std::string& outputPathname,
        const six::XMLControlRegistry& registry,
        const std::vector<std::string>& schemaPaths_)
{
    std::unique_ptr<six::sicd::ComplexData> complexData;
    std::vector<std::complex<float> > buffer;
    six::sicd::Utilities::readSicd(sicdPathname, schemaPaths_, complexData,
            buffer);

    if (!six::sicd::AreaPlaneUtility::hasAreaPlane(*complexData))
    {
        six::sicd::AreaPlaneUtility::setAreaPlane(*complexData);
    }

    six::NITFWriteControl writer;
    mem::SharedPtr<six::Container> container(new six::Container(six::DataType::COMPLEX));
    container->addData(complexData->clone());

    writer.initialize(container);
    writer.setXMLControlRegistry(&registry);
    std::vector<std::filesystem::path> schemaPaths;
    std::transform(schemaPaths_.begin(), schemaPaths_.end(), std::back_inserter(schemaPaths), [](const std::string& s) { return s; });
    save(writer, buffer, outputPathname, schemaPaths);
}

void roundTripXML(const std::string& sicdPathname,
        const std::string& outputPathname,
        const six::XMLControlRegistry& registry,
        const std::vector<std::string>& schemaPaths)
{
    std::unique_ptr<six::sicd::ComplexData> complexData =
        six::sicd::Utilities::getComplexData(sicdPathname, schemaPaths);

    if (!six::sicd::AreaPlaneUtility::hasAreaPlane(*complexData))
    {
        six::sicd::AreaPlaneUtility::setAreaPlane(*complexData);
    }

    logging::NullLogger log;
    std::unique_ptr<six::XMLControl> xmlControl(
            registry.newXMLControl(six::DataType::COMPLEX, &log));
    std::unique_ptr<xml::lite::Document> document(
            xmlControl->toXML(complexData.get(), schemaPaths));
    io::FileOutputStream xmlStream(outputPathname);
    document->getRootElement()->prettyPrint(xmlStream);
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <Input SICD> "
                    "<Output pathname>\n";
            return 1;
        }

        const std::string sicdPathname(argv[1]);
        const std::string outputPathname(argv[2]);
        const std::vector<std::string> schemaPaths;

        six::XMLControlRegistry registry;
        registry.addCreator<six::sicd::ComplexXMLControl>();

        if (str::endsWith(sicdPathname, ".nitf") ||
                str::endsWith(sicdPathname, ".ntf"))
        {
            roundTripNITF(sicdPathname, outputPathname, registry, schemaPaths);
        }
        else
        {
            roundTripXML(sicdPathname, outputPathname, registry, schemaPaths);
        }
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "An unknown error occured\n";
    }
    return 1;
}

