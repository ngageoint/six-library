/* =========================================================================
* This file is part of the CSM SICD Plugin
* =========================================================================
*
* (C) Copyright 2004 - 2014, MDA Information Systems LLC
*
* The CSM SICD Plugin is free software; you can redistribute it and/or modify
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
* License along with this program; if not,
* see <http://www.gnu.org/licenses/>.
*
*/

#define SIX_CSM_LIBRARY

#include <memory>

#include <six/csm/SIXPlugin.h>
#include <six/csm/SICDSensorModel.h>
#include <six/csm/SIDDSensorModel.h>
#include "Error.h"

#include <str/Manip.h>
#include <six/NITFReadControl.h>

namespace six
{
namespace CSM
{
const char SIXPlugin::PLUGIN_NAME[] = "SIX";
const char SIXPlugin::MANUFACTURER[] = "Radiant Solutions";
const char SIXPlugin::RELEASE_DATE[] = "20180818";
const size_t SIXPlugin::SICD_MODEL_INDEX;
const size_t SIXPlugin::SIDD_MODEL_INDEX;

const SIXPlugin SIXPlugin::mPlugin;

SIXPlugin::SIXPlugin()
{
}

std::string SIXPlugin::getPluginName() const
{
    return PLUGIN_NAME;
}

std::string SIXPlugin::getManufacturer() const
{
    return MANUFACTURER;
}

std::string SIXPlugin::getReleaseDate() const
{
    return RELEASE_DATE;
}

csm::Version SIXPlugin::getCsmVersion() const
{
    return ::CURRENT_CSM_VERSION;
}

size_t SIXPlugin::getNumModels() const
{
    return 2;
}

std::string SIXPlugin::getModelName(size_t modelIndex) const
{
    switch (modelIndex)
    {
    case SICD_MODEL_INDEX:
        return SICDSensorModel::NAME;
    case SIDD_MODEL_INDEX:
        return SIDDSensorModel::NAME;
    default:
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                           "Model index is " + str::toString(modelIndex),
                            "SIXPlugin::getModelName");
    }
}

std::string SIXPlugin::getModelFamily(size_t modelIndex) const
{
    switch (modelIndex)
    {
    case SICD_MODEL_INDEX:
        return SICDSensorModel::FAMILY;
    case SIDD_MODEL_INDEX:
        return SIDDSensorModel::FAMILY;
    default:
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                           "Model index is " + str::toString(modelIndex),
                            "SIXPlugin::getModelFamily");
    }
}

csm::Version SIXPlugin::getModelVersion(const std::string& modelName) const
{
    if (modelName == SICDSensorModel::NAME)
    {
        return SICDSensorModel::VERSION;
    }
    else if (modelName == SIDDSensorModel::NAME)
    {
        return SIDDSensorModel::VERSION;
    }
    else
    {
        throw csm::Error(csm::Error::SENSOR_MODEL_NOT_SUPPORTED,
                           "Model name is " + modelName,
                           "SIXPlugin::getModelVersion");
    }
}

bool SIXPlugin::canModelBeConstructedFromState(const std::string& modelName,
                                               const std::string& modelState,
                                               csm::WarningList* ) const
{
    // Assuming that if the model state starts with the model name, we're good
    // The only way to tell for sure would be to really construct a model which
    // would defeat the purpose of this method (since it's going to be called
    // each time prior to really constructing a model, so we'd end up doubling
    // the time it takes).
    return ((modelName == SICDSensorModel::NAME ||
             modelName == SIDDSensorModel::NAME) &&
            str::startsWith(modelState, modelName + " "));
}

bool SIXPlugin::canModelBeConstructedFromISD(const csm::Isd& imageSupportData,
                                              const std::string& modelName,
                                              csm::WarningList* ) const
{
    // Determine the data type that corresponds to this model
    six::DataType modelDataType;
    if (modelName == SICDSensorModel::NAME)
    {
        modelDataType = six::DataType::COMPLEX;
    }
    else if (modelName == SIDDSensorModel::NAME)
    {
        modelDataType = six::DataType::DERIVED;
    }
    else
    {
        return false;
    }

    // Same as above - don't want to do a totally robust check here or we'll
    // double the sensor model construction time.
    const std::string& format(imageSupportData.format());
    if (format == "NITF2.1")
    {
        // NOTE: Used dynamic_cast here previously but using it with IAI's
        //       version of vts, it threw an exception.  Did older versions of
        //       vts not use the /GR flag (they've since reported it worked
        //       fine when our plugin used dynamic_cast)?
        const csm::Nitf21Isd& nitfIsd =
                (const csm::Nitf21Isd&)imageSupportData;

        return ((modelDataType == six::DataType::COMPLEX &&
                     SICDSensorModel::containsComplexDES(nitfIsd)) ||
                (modelDataType == six::DataType::DERIVED &&
                     SIDDSensorModel::containsDerivedDES(nitfIsd)));
    }
    else if (format == "FILENAME")
    {
        // Note: this case has not been tested
        six::NITFReadControl readControl;
        try
        {
            const six::DataType fileDataType =
                    readControl.getDataType(imageSupportData.filename());

            return (fileDataType == modelDataType);
        }
        catch (const except::Exception& )
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

SIX_CSM_EXPORT_API csm::Model* SIXPlugin::constructModelFromState(
   const std::string& modelState,
   csm::WarningList* warnings) const
{
    const std::string sensorModelName =
            getModelNameFromModelState(modelState, warnings);

    // Based on the name, construct the appropriate sensor model
    try
    {
        if (sensorModelName == SICDSensorModel::NAME)
        {
            return new SICDSensorModel(modelState, getDataDirectory());
        }
        else if (sensorModelName == SIDDSensorModel::NAME)
        {
            return new SIDDSensorModel(modelState, getDataDirectory());
        }
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                           ex.toString(),
                           "SIXPlugin::constructModelFromState");
    }

    // Must be an invalid sensor model name
    throw csm::Error(csm::Error::INVALID_SENSOR_MODEL_STATE,
                       "Invalid sensor model state: " + sensorModelName,
                       "SIXPlugin::constructModelFromState");
}

SIX_CSM_EXPORT_API csm::Model* SIXPlugin::constructModelFromISD(
       const csm::Isd& imageSupportData,
       const std::string& modelName,
       csm::WarningList* ) const
{
    if (modelName == SICDSensorModel::NAME)
    {
        return new SICDSensorModel(imageSupportData, getDataDirectory());
    }
    else if (modelName == SIDDSensorModel::NAME)
    {
        return new SIDDSensorModel(imageSupportData, getDataDirectory());
    }
    else
    {
        throw csm::Error(csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                           "Invalid model name " + modelName,
                           "SIXPlugin::constructModelFromISD");
    }
}

std::string SIXPlugin::getModelNameFromModelState(
        const std::string& modelState,
        csm::WarningList* ) const
{
    if (str::startsWith(modelState, SICDSensorModel::NAME))
    {
        return SICDSensorModel::NAME;
    }
    else if (str::startsWith(modelState, SIDDSensorModel::NAME))
    {
        return SIDDSensorModel::NAME;
    }
    else
    {
        throw csm::Error(csm::Error::INVALID_SENSOR_MODEL_STATE,
                           "Could not get sensor model name",
                           "SIXPlugin::getModelNameFromModelState");
    }
}

bool SIXPlugin::canISDBeConvertedToModelState(const csm::Isd& imageSupportData,
                                               const std::string& modelName,
                                               csm::WarningList* warnings) const
{
    return canModelBeConstructedFromISD(imageSupportData, modelName, warnings);
}

std::string SIXPlugin::convertISDToModelState(
       const csm::Isd& imageSupportData,
       const std::string& modelName,
       csm::WarningList* warnings) const
{
    std::auto_ptr<const csm::Model>
            model(constructModelFromISD(imageSupportData, modelName, warnings));
    return model->getModelState();
}
}
}
