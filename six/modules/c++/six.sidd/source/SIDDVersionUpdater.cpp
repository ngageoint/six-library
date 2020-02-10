/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include <logging/Logger.h>
#include <scene/SceneGeometry.h>
#include <six/VersionUpdater.h>
#include <six/sidd/SIDDVersionUpdater.h>
#include <six/sidd/Utilities.h>
#include <cassert>

namespace
{
void populateFilter(six::sidd::Filter& filter)
{
    filter.filterName = "Placeholder";
    filter.filterKernel.reset(new six::sidd::Filter::Kernel());
    filter.filterKernel->custom.reset(new six::sidd::Filter::Kernel::Custom());
    filter.filterKernel->custom->size = six::RowColInt(1, 1);
    filter.filterKernel->custom->filterCoef.resize(
            filter.filterKernel->custom->size.area());
    filter.operation = six::sidd::FilterOperation::CONVOLUTION;
}
}

namespace six
{
namespace sidd
{
SIDDVersionUpdater::SIDDVersionUpdater(DerivedData& derivedData,
                                       const std::string& targetVersion,
                                       logging::Logger& log) :
    VersionUpdater(derivedData, targetVersion, getValidVersions(), log),
    mData(derivedData),
    mProcessingModuleIndex(0)
{
}

const std::vector<std::string>& SIDDVersionUpdater::getValidVersions()
{
    static const std::vector<std::string> siddVersions = {"1.0.0", "2.0.0"};
    return siddVersions;
}

void SIDDVersionUpdater::recordProcessingStep()
{
    if (!mData.productProcessing)
    {
        mData.productProcessing.reset(new ProductProcessing());
    }

    // Add a new processing block to tell consumers about
    // the automated version update.
    mem::ScopedCopyablePtr<ProcessingModule> versionProcessing(
            new ProcessingModule());
    versionProcessing->moduleName = "Automated version update";

    mProcessingModuleIndex = mData.productProcessing->processingModules.size();
    mData.productProcessing->processingModules.push_back(versionProcessing);
}

void SIDDVersionUpdater::addProcessingParameter(const std::string& fieldName)
{
    Parameter parameter(fieldName);
    parameter.setName("Guessed Field");
    auto processingModule =
            mData.productProcessing->processingModules[mProcessingModuleIndex];
    processingModule->moduleParameters.push_back(parameter);
}

void SIDDVersionUpdater::updateSingleIncrement()
{
    const std::string thisVersion = mData.getVersion();
    if (thisVersion == "1.0.0")
    {
        // Classification
        if (mData.productCreation->classification.compliesWith.empty())
        {
            mData.productCreation->classification.compliesWith.push_back(
                    "OtherAuthority");
            emitWarning("ProductCreation.Classification.CompliesWith");
        }
        // GeographicAndTarget
        mData.geographicAndTarget->imageCorners.reset(new LatLonCorners());
        *mData.geographicAndTarget->imageCorners =
                mData.geographicAndTarget->geographicCoverage->footprint;

        // TODO: Since validData doesn't seem to be present in SIDD 1.0,
        // I'm assuming that the validData and imageCorners would be
        // the same?
        // This might also come from TargetInformation.footprint
        mData.geographicAndTarget->validData.resize(4);
        for (size_t ii = 0; ii < 4; ++ii)
        {
            mData.geographicAndTarget->validData[ii] =
                    mData.geographicAndTarget->imageCorners->getCorner(ii);
        }
        emitWarning("GeographicAndTarget.ValidData");

        mData.geographicAndTarget->geographicCoverage.reset();
        mData.geographicAndTarget->targetInformation.clear();

        // Measurement
        // See comment for GeographicAndTarget.ValidData
        const RowColInt& footprint = mData.measurement->pixelFootprint;
        mData.measurement->validData.resize(4);
        mData.measurement->validData[0] = RowColInt(0, 0);
        mData.measurement->validData[1] = RowColInt(footprint.row, 0);
        mData.measurement->validData[2] =
                RowColInt(footprint.row, footprint.col);
        mData.measurement->validData[3] = RowColInt(0, footprint.col);
        emitWarning("Measurement.ValidData");

        if (mData.exploitationFeatures)
        {
            for (size_t ii = 0; ii < mData.exploitationFeatures->product.size();
                 ++ii)
            {
                Product& product = mData.exploitationFeatures->product[ii];
                product.ellipticity = 0;
                ProcTxRcvPolarization polarization;
                polarization.txPolarizationProc =
                        PolarizationSequenceType::OTHER;
                polarization.rcvPolarizationProc =
                        PolarizationSequenceType::OTHER;
                product.polarization.push_back(polarization);

                std::ostringstream msg;
                msg << "ExploitationFeatures.Product[" << ii << "].Ellipcitiy";
                emitWarning(msg.str());

                msg.str("");
                msg << "ExploitationFeatures.Product[" << ii
                    << "].Polarization";
                emitWarning(msg.str());
            }
        }

        // Display
        mData.display->nonInteractiveProcessing.resize(1);
        mData.display->nonInteractiveProcessing[0].reset(
                new six::sidd::NonInteractiveProcessing());
        mData.display->nonInteractiveProcessing[0]->rrds.downsamplingMethod =
                DownsamplingMethod::DECIMATE;

        mData.display->nonInteractiveProcessing[0]
                ->productGenerationOptions.dataRemapping.reset(
                        new LookupTable());
        auto& lut = *mData.display->nonInteractiveProcessing[0]
                             ->productGenerationOptions.dataRemapping;
        lut.lutName = "Placeholder";
        lut.custom.reset(new LookupTable::Custom(1, 1));
        emitWarning("Display.NoninteractiveProcessing");

        mData.display->interactiveProcessing.resize(1);
        mData.display->interactiveProcessing[0].reset(
                new six::sidd::InteractiveProcessing());
        auto& filter = mData.display->interactiveProcessing[0]
                               ->geometricTransform.scaling.antiAlias;
        populateFilter(filter);
        populateFilter(mData.display->interactiveProcessing[0]
                               ->geometricTransform.scaling.interpolation);
        mData.display->interactiveProcessing[0]
                ->geometricTransform.orientation.shadowDirection =
                ShadowDirection::RIGHT;
        mData.display->interactiveProcessing[0]
                ->sharpnessEnhancement.modularTransferFunctionEnhancement.reset(
                        new Filter());
        populateFilter(*mData.display->interactiveProcessing[0]
                                ->sharpnessEnhancement
                                .modularTransferFunctionEnhancement);
        mData.display->interactiveProcessing[0]
                ->dynamicRangeAdjustment.algorithmType = DRAType::NONE;
        emitWarning("Display.InteractiveProcessing");
    }
    else
    {
        throw except::Exception(Ctxt("Unhandled version: " + thisVersion));
    }
}
}
}
