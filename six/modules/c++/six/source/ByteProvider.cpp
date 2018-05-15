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

void ByteProvider::populateWriter(
        mem::SharedPtr<Container> container,
        const XMLControlRegistry& xmlRegistry,
        size_t maxProductSize,
        size_t numRowsPerBlock,
        size_t numColsPerBlock,
        NITFWriteControl& writer)
{
    if (container->getNumData() != 1)
    {
        throw except::Exception(Ctxt(
                "Expected data of size 1 in container but got " +
                str::toString(container->getNumData())));
    }

    const six::Data* const data = container->getData(0);

    writer.setXMLControlRegistry(&xmlRegistry);

    if (maxProductSize != 0)
    {
        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE,
                maxProductSize);
    }

    if (numRowsPerBlock != 0)
    {
        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_NUM_ROWS_PER_BLOCK,
                numRowsPerBlock);
        numRowsPerBlock = std::min(numRowsPerBlock, data->getNumRows());
    }

    if (numColsPerBlock != 0)
    {
        writer.getOptions().setParameter(
                six::NITFWriteControl::OPT_NUM_COLS_PER_BLOCK,
                numColsPerBlock);
        numColsPerBlock = std::min(numColsPerBlock, data->getNumCols());
    }

    writer.initialize(container);
}

void ByteProvider::populateInitArgs(
        const NITFWriteControl& writer,
        const std::vector<std::string>& schemaPaths,
        std::vector<PtrAndLength>& desData,
        size_t& numRowsPerBlock,
        size_t& numColsPerBlock)
{
    // Sanity check the container
    mem::SharedPtr<const Container> container = writer.getContainer();

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
    std::vector<std::string> xmlStrings(container->getNumData());
    desData.resize(xmlStrings.size());
    for (size_t ii = 0; ii < xmlStrings.size(); ++ii)
    {
        std::string& xmlString(xmlStrings[ii]);
        xmlString = six::toValidXMLString(container->getData(ii),
                                          schemaPaths,
                                          &logger,
                                          writer.getXMLControlRegistry());
        desData[ii].first = xmlString.c_str();
        desData[ii].second = xmlString.length();
    }

    // Get blocking info
    const Options& options(writer.getOptions());
    const Parameter zero(0);

    numRowsPerBlock = static_cast<sys::Uint32_T>(
            options.getParameter(NITFWriteControl::OPT_NUM_ROWS_PER_BLOCK,
                                 zero));

    numColsPerBlock = static_cast<sys::Uint32_T>(
            options.getParameter(NITFWriteControl::OPT_NUM_COLS_PER_BLOCK,
                                 zero));


}

void ByteProvider::initialize(mem::SharedPtr<Container> container,
                              const XMLControlRegistry& xmlRegistry,
                              const std::vector<std::string>& schemaPaths,
                              size_t maxProductSize,
                              size_t numRowsPerBlock,
                              size_t numColsPerBlock)
{
    NITFWriteControl writer;
    populateWriter(container, xmlRegistry, maxProductSize, numRowsPerBlock,
            numColsPerBlock, writer);
    initialize(writer, schemaPaths);
}

void ByteProvider::initialize(const NITFWriteControl& writer,
                              const std::vector<std::string>& schemaPaths)
{
    std::vector<PtrAndLength> desData;
    size_t numRowsPerBlock;
    size_t numColsPerBlock;
    populateInitArgs(writer,
                     schemaPaths,
                     desData,
                     numRowsPerBlock,
                     numColsPerBlock);

    // Do the full initialization
    nitf::Record record = writer.getRecord();
    nitf::ByteProvider::initialize(record,
                                   desData,
                                   numRowsPerBlock,
                                   numColsPerBlock);
}
}
