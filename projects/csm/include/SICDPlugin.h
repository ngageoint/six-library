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

#ifndef __STUBSMPLUGIN_H
#define __STUBSMPLUGIN_H

/**
 * @class SICDPlugin
 *
 * @brief This plugin implements the TSMPlugin API for SICD data
 *
 * @author Martina Schultz
 */

#include <list>
#include "TSMPlugin.h"
#include "TSMWarning.h"
#include "TSMError.h"
#include "TSMSensorModel.h"

#ifdef _WIN32
# ifdef SICD_LIBRARY
#  define SICD_EXPORT_API __declspec(dllexport)
# else
#  define SICD_EXPORT_API __declspec(dllimport)
# endif
#else
#  define SICD_EXPORT_API
#endif

#define PLUGIN_API(_X) virtual SICD_EXPORT_API TSMWarning* _X
#define THROWS const throw (TSMError)

namespace sicd_plugin
{

class SICDPlugin: public TSMPlugin
{
public:

    virtual SICD_EXPORT_API ~SICDPlugin()
    {
    }

    PLUGIN_API(getManufacturer) (std::string& name) THROWS;

    PLUGIN_API(getReleaseDate) (std::string& releaseDate) THROWS;

    PLUGIN_API(getNSensorModels)(int& nModels) THROWS;

    PLUGIN_API(getSensorModelName)(const int& sensorModelIndex,
                                   std::string& sensorModelName) THROWS;

    PLUGIN_API(getSensorModelVersion)(const std::string& sensorModelName,
                                      int& version) THROWS;

    PLUGIN_API(canSensorModelBeConstructedFromState)
            (const std::string& sensorModelName,
             const std::string& sensorModelState,
             bool& constructible) THROWS;

    PLUGIN_API(canSensorModelBeConstructedFromISD)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             bool& constructible) THROWS;

    PLUGIN_API(constructSensorModelFromState)
            (const std::string& sensorModelState,
             TSMSensorModel*& sensor_model) THROWS;

    PLUGIN_API(constructSensorModelFromISD)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             TSMSensorModel*& sensor_model) THROWS;

    PLUGIN_API(getSensorModelNameFromSensorModelState)
            (const std::string& sensorModelState,
             std::string& sensorModelName) THROWS;

    PLUGIN_API(canISDBeConvertedToSensorModelState)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             bool& convertible) THROWS;

    PLUGIN_API(convertISDToSensorModelState)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             std::string& sensorModelState) THROWS;

    PLUGIN_API(getPluginName)(std::string& pluginName) THROWS;

private:
    typedef std::list<TSMSensorModel*> SICDModels;

    SICDPlugin();

    static SICD_EXPORT_API const char PLUGIN_NAME[];
    static SICD_EXPORT_API const char SENSOR_MODEL_NAME[];
    static SICD_EXPORT_API const char MANUFACTURER[];
    static SICD_EXPORT_API const char RELEASE_DATE[];
    static SICD_EXPORT_API const int VERSION;
    static const SICDPlugin mPlugin;
};

}

#endif
