/* =========================================================================
 * This file is part of six-c++
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

#include <cli/ArgumentParser.h>
#include <sys/Conf.h>

#include <mem/SharedPtr.h>
#include <mem/ScopedAlignedArray.h>
#include <except/Exception.h>

#include <six/Data.h>
#include <six/Types.h>
#include <six/sicd/ComplexXMLControl.h>

#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/DerivedClassification.h>
#include <six/sidd/ProductCreation.h>
#include <six/sidd/DownstreamReprocessing.h>
#include <six/sidd/ExploitationFeatures.h>

#include <cstring>

namespace
{
std::shared_ptr<six::Data> readMetadata(const six::NITFReadControl& reader)
{
    const six::Data* data = reader.getContainer()->getData(0);
    std::shared_ptr<six::Data> retv(data->clone());
    return retv;
}

void readWideband(six::NITFReadControl& reader,
        const six::Data& data,
        mem::ScopedAlignedArray<six::UByte>& buffer)
{
    buffer.reset(data.getNumRows() *
                 data.getNumCols() *
                 data.getNumBytesPerPixel());
    six::Region region;
    region.setStartRow(0);
    region.setStartCol(0);
    region.setNumRows(data.getNumRows());
    region.setNumCols(data.getNumCols());
    region.setBuffer(buffer.get());
    reader.interleaved(region, 0);
}

bool siddsMatch(const std::string& sidd1Path,
        const std::string& sidd2Path,
        bool ignoreMetadata,
        bool ignoreDate)
{
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<
            six::sicd::ComplexXMLControl>());
    xmlRegistry.addCreator(six::DataType::DERIVED,
            new six::XMLControlCreatorT<
            six::sidd::DerivedXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    reader.load(sidd1Path);
    std::shared_ptr<six::Data> sidd1Metadata = readMetadata(reader);
    mem::ScopedAlignedArray<six::UByte> sidd1Buffer;
    readWideband(reader,
            *sidd1Metadata,
            sidd1Buffer);

    reader.load(sidd2Path);
    std::shared_ptr<six::Data> sidd2Metadata = readMetadata(reader);
    mem::ScopedAlignedArray<six::UByte> sidd2Buffer;
    readWideband(reader,
            *sidd2Metadata,
            sidd2Buffer);

    if (!ignoreMetadata)
    {
        if (ignoreDate)
        {
            // Creation time is easy
            sidd2Metadata->setCreationTime(sidd1Metadata->getCreationTime());

            // Various SICD/SIDD specific fields
            six::sidd::DerivedData* ddata1 =
                dynamic_cast<six::sidd::DerivedData*>(sidd1Metadata.get());
            six::sidd::DerivedData* ddata2 =
                dynamic_cast<six::sidd::DerivedData*>(sidd2Metadata.get());

            if (ddata1 && ddata2)
            {
                // Processing events

                // aliases for long names...
                six::sidd::DownstreamReprocessing* dr1 =
                    ddata1->downstreamReprocessing.get();
                six::sidd::DownstreamReprocessing* dr2 =
                    ddata2->downstreamReprocessing.get();

                size_t nEvents1 = dr1->processingEvents.size();
                size_t nEvents2 = dr2->processingEvents.size();
                if (nEvents1 != nEvents2) return false;
                for (size_t ii = 0; ii < nEvents1; ++ii)
                {
                    dr2->processingEvents[ii]->appliedDateTime =
                        dr1->processingEvents[ii]->appliedDateTime;
                }

                // Collection information

                // aliases for long names...
                six::sidd::ExploitationFeatures* ef1 =
                    ddata1->exploitationFeatures.get();
                six::sidd::ExploitationFeatures* ef2 =
                    ddata2->exploitationFeatures.get();
                size_t nCollect1 = ef1->collections.size();
                size_t nCollect2 = ef2->collections.size();
                if (nCollect1 != nCollect2) return false;
                for (size_t ii = 0; ii < nCollect1; ++ii)
                {
                    ef2->collections[ii]->information.collectionDateTime =
                        ef1->collections[ii]->information.collectionDateTime;
                    ef2->collections[ii]->information.localDateTime =
                        ef1->collections[ii]->information.localDateTime;
                }

                // Derived specific classification times
                six::sidd::DerivedClassification& class1 =
                    ddata1->productCreation->classification;
                six::sidd::DerivedClassification& class2 =
                    ddata2->productCreation->classification;
                class2.createDate = class1.createDate;
                class2.exemptedSourceDate = class1.exemptedSourceDate;
                class2.declassDate = class1.declassDate;
            }
            else
            {
                // TODO: SICD specific time fields
            }
        }

        bool metadataMatches = (*sidd1Metadata) == (*sidd2Metadata);

        if (!metadataMatches)
        {
            return false;
        }
    }
    else
    {
        // If we're ignoring metadata we need to explicitly check image data
        // dimensions to prevent memcmp() from going out of bounds
        if (sidd1Metadata->getNumRows() != sidd2Metadata->getNumRows() ||
            sidd1Metadata->getNumCols() != sidd2Metadata->getNumCols() ||
            sidd1Metadata->getNumBytesPerPixel() !=
                sidd2Metadata->getNumBytesPerPixel())
        {
            return false;
        }
    }

    int result =
        std::memcmp(sidd1Buffer.get(), sidd2Buffer.get(),
                sidd1Metadata->getNumRows() *
                sidd1Metadata->getNumCols() *
                sidd1Metadata->getNumBytesPerPixel());

    return !result;
}
}

int main(int argc, char* argv[])
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription("This program compares two SIDDs to determine "
                "if they are identical or not. If the SIDDs match, the "
                "program's exit code will be 0. If the SIDDs don't match,"
                "or if there is an error, the code will be nonzero.");
        parser.addArgument("--ignore-metadata",
                "Does not check SIDD metadata for differences",
                cli::STORE_TRUE, "ignoreMetadata")->setDefault("false");
        parser.addArgument("--ignore-date",
                "Does not check SIDD creation date for differences",
                cli::STORE_TRUE, "ignoreDate")->setDefault("false");
        parser.addArgument("sidd1", "Input SIDD path",
                cli::STORE, "SIDD1", "SIDD1", 1, 1, true);
        parser.addArgument("sidd2", "Input SIDD path",
                cli::STORE, "SIDD2", "SIDD2", 1, 1, true);

        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));

        const bool ignoreMetadata(options->get<bool>("ignoreMetadata"));
        const bool ignoreDate(options->get<bool>("ignoreDate"));
        const std::string sidd1Path(options->get<std::string>("SIDD1"));
        const std::string sidd2Path(options->get<std::string>("SIDD2"));

        if (siddsMatch(sidd1Path, sidd2Path, ignoreMetadata, ignoreDate))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
    catch (const except::Throwable& exception)
    {
        std::cerr << exception.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
