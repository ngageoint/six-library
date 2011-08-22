/* =========================================================================
* This file is part of the CSM SICD Plugin
* =========================================================================
*
* (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
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

#define SICD_LIBRARY

#include <algorithm>
#include <cstring>
#include <string>
#include <iostream>
#include "SICDPlugin.h"
#include "SICDSensorModel.h"
#include "TSMWarning.h"
#include "TSMError.h"
#include "TSMISDFilename.h"

using namespace sicd_plugin;

const char SICDPlugin::PLUGIN_NAME[] = "SICDPlugin";
const char SICDPlugin::SENSOR_MODEL_NAME[] = "SICD_SENSOR_MODEL";
const char SICDPlugin::MANUFACTURER[] = "GDAIS";
const char SICDPlugin::RELEASE_DATE[] = "20110729";
const int SICDPlugin::VERSION = 1;

const SICDPlugin SICDPlugin::mPlugin;

SICDPlugin::SICDPlugin()
{
}

TSMWarning *SICDPlugin::constructSensorModelFromState(
                                                      const std::string& sensorModelState,
                                                      TSMSensorModel*& sensorModel) const
                                                                                          throw (TSMError)
{
    sensorModel = NULL;
    std::string funcName("SICDPlugin::constructSensorModelFromState");

    try
    {
        // construct sensor model from XML string
        sensorModel = new sicd_sensor::SICDSensorModel(sensorModelState);
    }
    catch (except::Exception& ex)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::SENSOR_MODEL_NOT_CONSTRUCTIBLE, 
                           ex.toString(), 
                           funcName);
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDPlugin::constructSensorModelFromISD(
                                                    const tsm_ISD& imageSupportData,
                                                    const std::string& sensorModelName,
                                                    TSMSensorModel*& sensorModel) const
                                                                                        throw (TSMError)
{
    sensorModel = NULL;
    std::string funcName("SICDPlugin::constructSensorModelFromISD");
    std::string format;
    imageSupportData.getFormat(format);

    try
    {
        if (format == "NITF2.1")
        {
            NITF_2_1_ISD *tsmNITF = (NITF_2_1_ISD*) &imageSupportData;
            sensorModel = new sicd_sensor::SICDSensorModel(tsmNITF,
                                                           sensorModelName);
        }
        else if (format == "FILENAME")
        {
            // Note: this case has not been tested
            filenameISD *tsmFilename = (filenameISD*) &imageSupportData;
            sensorModel
                    = new sicd_sensor::SICDSensorModel(tsmFilename->_filename,
                                                       sensorModelName);
        }
        else
            throw except::Exception("Unsupported ISD format");
    }
    catch (except::Exception& ex)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::ISD_NOT_SUPPORTED, ex.toString(), funcName);
        throw tsmErr;
    }
    return NULL;
}

TSMWarning *SICDPlugin::convertISDToSensorModelState(
                                                     const tsm_ISD& imageSupportData,
                                                     const std::string& sensorModelName,
                                                     std::string& returnedSensorModelState) const
                                                                                                  throw (TSMError)
{
    std::string funcName("SICDPlugin::convertISDToSensorModelState");
    returnedSensorModelState = "";
    try
    {
        bool convertible = false;
        canISDBeConvertedToSensorModelState(imageSupportData,
                                            sensorModelName, convertible);

        if (convertible)
        {
            TSMSensorModel* sensorModel = NULL;
            constructSensorModelFromISD(imageSupportData,
                                        sensorModelName, sensorModel);
            sensorModel->getSensorModelState(returnedSensorModelState);
            delete sensorModel;
        }
        else
            throw except::Exception("Sensor Model not constructible from ISD");
    }
    catch (except::Exception& ex)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::SENSOR_MODEL_NOT_CONSTRUCTIBLE, 
                ex.toString(), funcName);
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDPlugin::canISDBeConvertedToSensorModelState(
                                                            const tsm_ISD& imageSupportData,
                                                            const std::string& sensorModelName,
                                                            bool& convertible) const
                                                                                     throw (TSMError)
{
    convertible = false;
    canSensorModelBeConstructedFromISD(imageSupportData, 
                                       sensorModelName, convertible);

    return NULL;
}

TSMWarning *SICDPlugin::canSensorModelBeConstructedFromState(
                                                             const std::string& sensorModelName,
                                                             const std::string& sensorModelState,
                                                             bool& constructible) const
                                                                                        throw (TSMError)
{
    constructible = false;

    try
    {
        if (str::startsWith(sensorModelState, sensorModelName))
        {
            TSMSensorModel* sensorModel = 
                    new sicd_sensor::SICDSensorModel(sensorModelState);
            delete sensorModel;
            constructible = true;
        }
    }
    catch (except::Exception& ex)
    {
        constructible = false;
    }

    return NULL;
}

TSMWarning *SICDPlugin::canSensorModelBeConstructedFromISD(
                                                           const tsm_ISD& imageSupportData,
                                                           const std::string& sensorModelName,
                                                           bool& constructible) const
                                                                                      throw (TSMError)
{
    constructible = false;

    try
    {
        if (sensorModelName == SENSOR_MODEL_NAME)
        {
            std::string format;
            imageSupportData.getFormat(format);

            if (format == "NITF2.1")
            {
                NITF_2_1_ISD *tsmNITF = (NITF_2_1_ISD*) &imageSupportData;
                TSMSensorModel* sensorModel = 
                        new sicd_sensor::SICDSensorModel(tsmNITF,
                                                         sensorModelName);
                delete sensorModel;
                constructible = true;
            }
            else if (format == "FILENAME")
            {
                filenameISD *tsmFilename = (filenameISD*) &imageSupportData;
                TSMSensorModel* sensorModel =
                        new sicd_sensor::SICDSensorModel(tsmFilename->_filename,
                                                         sensorModelName);
                delete sensorModel;
                constructible = true;
            }
        }
    }
    catch (except::Exception& ex)
    {
        constructible = false;
    }

    return NULL;
}

TSMWarning *SICDPlugin::getManufacturer(std::string& manufacturer) const
                                                                         throw (TSMError)
{
    manufacturer = MANUFACTURER;
    return NULL;
}

TSMWarning *SICDPlugin::getNSensorModels(int& nModels) const throw (TSMError)
{
    nModels = 1;
    return NULL;
}

TSMWarning *SICDPlugin::getReleaseDate(std::string& releaseDate) const
                                                                       throw (TSMError)
{
    releaseDate = RELEASE_DATE;
    return NULL;
}

TSMWarning *SICDPlugin::getSensorModelName(const int& index,
                                           std::string& sensorModelName) const
                                                                               throw (TSMError)
{
    sensorModelName = SENSOR_MODEL_NAME;
    return NULL;
}

TSMWarning *SICDPlugin::getSensorModelNameFromSensorModelState(
                                                               const std::string& sensorModelState,
                                                               std::string& sensorModelName) const
                                                                                                   throw (TSMError)
{
    sensorModelName = "";
    std::string funcName("SICDPlugin::getSensorModelNameFromSensorModelState");

    if (str::startsWith(sensorModelState, std::string(SENSOR_MODEL_NAME)))
        sensorModelName = SENSOR_MODEL_NAME;
    else
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::INVALID_SENSOR_MODEL_STATE, 
                           "Could not get sensor model name", 
                           funcName);
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDPlugin::getSensorModelVersion(
                                              const std::string& sensorModelName,
                                              int& version) const
                                                                  throw (TSMError)
{
    version = VERSION;
    return NULL;
}

TSMWarning *SICDPlugin::getPluginName(std::string& pluginName) const
                                                                     throw (TSMError)
{
    pluginName = PLUGIN_NAME;
    return NULL;
}
