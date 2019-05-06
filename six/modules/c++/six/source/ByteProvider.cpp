/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <str/Convert.h>
#include <logging/NullLogger.h>
#include <six/ByteProvider.h>

namespace six
{

ByteProvider::ByteProvider()
{
}

ByteProvider::ByteProvider(std::auto_ptr<six::NITFHeaderCreator> headerCreator,
                           const std::vector<std::string>& schemaPaths,
                           const std::vector<PtrAndLength>& desBuffers)
{
    initialize(headerCreator, schemaPaths, desBuffers);
}

void ByteProvider::populateOptions(
        mem::SharedPtr<Container> container,
        size_t maxProductSize,
        size_t numRowsPerBlock,
        size_t numColsPerBlock,
        Options& options)
{
    if (container->getNumData() != 1)
    {
        throw except::Exception(Ctxt(
                "Expected data of size 1 in container but got " +
                str::toString(container->getNumData())));
    }

    const six::Data* const data = container->getData(0);


    if (maxProductSize != 0)
    {
        options.setParameter(
                six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                maxProductSize);
    }

    if (numRowsPerBlock != 0)
    {
        numRowsPerBlock = std::min(numRowsPerBlock, data->getNumRows());
        options.setParameter(
                six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK,
                numRowsPerBlock);
    }

    if (numColsPerBlock != 0)
    {
        numColsPerBlock = std::min(numColsPerBlock, data->getNumCols());
        options.setParameter(
                six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK,
                numColsPerBlock);
    }
}

void ByteProvider::populateInitArgs(
        const NITFWriteControl& writer,
        const std::vector<std::string>& schemaPaths,
        std::vector<std::string>& xmlStrings,
        std::vector<PtrAndLength>& desData,
        size_t& numRowsPerBlock,
        size_t& numColsPerBlock)
{
    const NITFHeaderCreator* headerCreator =
        writer.getNITFHeaderCreator();
    if (headerCreator)
    {
        populateInitArgs(*headerCreator,
                         schemaPaths,
                         xmlStrings,
                         desData,
                         numRowsPerBlock,
                         numColsPerBlock);
    }
    else
    {
        throw except::Exception(Ctxt("NITF writer is not populated"));
    }
}

void ByteProvider::populateInitArgs(
        const NITFHeaderCreator& headerCreator,
        const std::vector<std::string>& schemaPaths,
        std::vector<std::string>& xmlStrings,
        std::vector<PtrAndLength>& desData,
        size_t& numRowsPerBlock,
        size_t& numColsPerBlock)
{
    // Sanity check the container
    mem::SharedPtr<const Container> container = headerCreator.getContainer();

    if (container->getNumData() == 0)
    {
        throw except::Exception(Ctxt(
                "Write control must be initialized first"));
    }

    // We currently do not support the case where there are 2+ unrelated SIDDs
    // in a file (but one logical SIDD which spans multiple image segments
    // and/or contains SICD XML is ok).  This is a limitation in how
    // nitf::ByteProvider computes row offsets.  So, ensure this constraint is
    // met.
    bool haveDerived(false);
    for (size_t ii = 0; ii < container->getNumData(); ++ii)
    {
        if (container->getData(ii)->getDataType() == DataType::DERIVED)
        {
            if (!haveDerived)
            {
                haveDerived = true;
            }
            else
            {
                throw except::Exception(Ctxt(
                        "Don't currently support more than one SIDD image"));
            }
        }
    }

    // Create XML strings
    // This memory must stay around until the call to the
    // base class's initialize() method
    logging::NullLogger logger;
    xmlStrings.resize(container->getNumData());
    desData.resize(xmlStrings.size());
    for (size_t ii = 0; ii < xmlStrings.size(); ++ii)
    {
        std::string& xmlString(xmlStrings[ii]);
        xmlString = six::toValidXMLString(container->getData(ii),
                                          schemaPaths,
                                          &logger,
                                          headerCreator.getXMLControlRegistry());
        desData[ii].first = xmlString.c_str();
        desData[ii].second = xmlString.length();
    }

    // Get blocking info
    const Options& options(headerCreator.getOptions());
    const Parameter zero(0);

    numRowsPerBlock = static_cast<sys::Uint32_T>(
            options.getParameter(NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK,
                                 zero));

    numColsPerBlock = static_cast<sys::Uint32_T>(
            options.getParameter(NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK,
                                 zero));
}

void ByteProvider::initialize(mem::SharedPtr<Container> container,
                              const XMLControlRegistry& xmlRegistry,
                              const std::vector<std::string>& schemaPaths,
                              size_t maxProductSize,
                              size_t numRowsPerBlock,
                              size_t numColsPerBlock)
{
    Options options;
    populateOptions(container, maxProductSize, numRowsPerBlock,
            numColsPerBlock, options);
    NITFWriteControl writer(options, container, &xmlRegistry);
    initialize(writer, schemaPaths);
}

void ByteProvider::initialize(const NITFWriteControl& writer,
                              const std::vector<std::string>& schemaPaths)
{
    std::vector<PtrAndLength> emptyDesBuffers;
    initialize(writer, schemaPaths, emptyDesBuffers);
}

void ByteProvider::initialize(const NITFWriteControl& writer,
                              const std::vector<std::string>& schemaPaths,
                              const std::vector<PtrAndLength>& desBuffers)
{
    // We don't explicitly use it, but each element in desData has a pointer
    // into this vector, so we need it to stick around
    std::vector<std::string> xmlStrings;
    std::vector<PtrAndLength> desData;
    size_t numRowsPerBlock;
    size_t numColsPerBlock;
    populateInitArgs(writer,
                     schemaPaths,
                     xmlStrings,
                     desData,
                     numRowsPerBlock,
                     numColsPerBlock);

    for (size_t ii = 0; ii < desBuffers.size(); ++ii)
    {
        desData.push_back(desBuffers[ii]);
    }

    // Do the full initialization
    nitf::Record record = writer.getRecord();
    nitf::ByteProvider::initialize(record,
                                   desData,
                                   numRowsPerBlock,
                                   numColsPerBlock);
}

void ByteProvider::initialize(std::auto_ptr<six::NITFHeaderCreator> headerCreator,
                              const std::vector<std::string>& schemaPaths,
                              const std::vector<PtrAndLength>& desBuffers)
{
    // We don't explicitly use it, but each element in desData has a pointer
    // into this vector, so we need it to stick around
    std::vector<std::string> xmlStrings;
    std::vector<PtrAndLength> desData;
    size_t numRowsPerBlock;
    size_t numColsPerBlock;
    populateInitArgs(*headerCreator.get(),
                     schemaPaths,
                     xmlStrings,
                     desData,
                     numRowsPerBlock,
                     numColsPerBlock);

    for (size_t ii = 0; ii < desBuffers.size(); ++ii)
    {
        desData.push_back(desBuffers[ii]);
    }

    // Do the full initialization
    nitf::Record& record = headerCreator->getRecord();
    nitf::ByteProvider::initialize(record,
                                   desData,
                                   numRowsPerBlock,
                                   numColsPerBlock);
}

}
