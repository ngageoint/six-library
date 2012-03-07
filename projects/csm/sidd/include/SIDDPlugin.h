/* =========================================================================
 * This file is part of the CSM SIDD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
 *
 * The CSM SIDD Plugin is free software; you can redistribute it and/or modify
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

#ifndef __SIDDSMPLUGIN_H
#define __SIDDSMPLUGIN_H

/**
 * @class SIDDPlugin
 *
 * @brief This plugin implements the TSMPlugin API for SIDD data
 *
 * @author Martina Schultz
 */

#include <list>
#include "TSMPlugin.h"
#include "TSMWarning.h"
#include "TSMError.h"
#include "TSMSensorModel.h"

#ifdef _WIN32
# ifdef SIDD_LIBRARY
#  define SIDD_EXPORT_API __declspec(dllexport)
# else
#  define SIDD_EXPORT_API __declspec(dllimport)
# endif
#else
#  define SIDD_EXPORT_API
#endif

#define PLUGIN_API(_X) virtual SIDD_EXPORT_API TSMWarning* _X
#define THROWS const throw (TSMError)

namespace sidd_plugin
{

class SIDDPlugin: public TSMPlugin
{
public:

    virtual SIDD_EXPORT_API ~SIDDPlugin()
    {
    }

    /**
     * Get manufacturer name.
     *
     * \param[out] name   The name of the plugin manufacturer
     */
    PLUGIN_API(getManufacturer) (std::string& name) THROWS;

    /**
     * Get release date.
     *
     * \param[out] releaseDate   The release date of the plugin
     */
    PLUGIN_API(getReleaseDate) (std::string& releaseDate) THROWS;

    /**
     * Get number of sensor models.
     *
     * \param[out] nModels   The number of sensor models supported by the plugin     */
    PLUGIN_API(getNSensorModels)(int& nModels) THROWS;

    /**
     * Get the name of the sensor model referenced by the supplied index.
     *
     * \param[in] sensorModelIndex   The index of the sensor model
     * \param[out] sensorModelName   The sensor model name
     */
    PLUGIN_API(getSensorModelName)(const int& sensorModelIndex,
                                   std::string& sensorModelName) THROWS;

    /**
     * Get the version of the sensor model identified by the supplied name.
     *
     * \param[in] sensorModelName   The sensor model name
     * \param[out] version   The sensor model version
     */
    PLUGIN_API(getSensorModelVersion)(const std::string& sensorModelName,
                                      int& version) THROWS;

    /**
     * Determine whether the sensor model identified by the supplied name can
     * be constructed from the given state string.
     *
     * \param[in] sensorModelName   The sensor model name
     * \param[in] sensorModelState  The sensor model state string
     * \param[out] constructible    True if the sensor model can be constructed
     *                              from the given parameters, otherwise false.
     */
    PLUGIN_API(canSensorModelBeConstructedFromState)
            (const std::string& sensorModelName,
             const std::string& sensorModelState,
             bool& constructible) THROWS;

    /**
     * Determine whether the sensor model identified by the supplied name can
     * be constructed from the given ISD object.
     *
     * \param[in] imageSupportData  The image support data object
     * \param[in] sensorModelName   The sensor model name
     * \param[out] constructible    True if the sensor model can be constructed
     *                              from the given parameters, otherwise false.
     */
    PLUGIN_API(canSensorModelBeConstructedFromISD)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             bool& constructible) THROWS;

    /**
     * Construct the sensor model from the given state string.
     *
     * \param[in] sensorModelState  The sensor model state string
     * \param[out] sensor_model     The sensor model object
     */
    PLUGIN_API(constructSensorModelFromState)
            (const std::string& sensorModelState,
             TSMSensorModel*& sensor_model) THROWS;

    /**
     * Construct the sensor model from the given ISD object.
     *
     * \param[in] imageSupportData  The image support data object
     * \param[in] sensorModelName   The sensor model name
     * \param[out] sensor_model     The sensor model object
     */
    PLUGIN_API(constructSensorModelFromISD)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             TSMSensorModel*& sensor_model) THROWS;

    /**
     * Obtain the sensor model name from the given state string.
     *
     * \param[in] sensorModelState  The sensor model state string
     * \param[out] sensorModelName  The sensor model name
     */
    PLUGIN_API(getSensorModelNameFromSensorModelState)
            (const std::string& sensorModelState,
             std::string& sensorModelName) THROWS;

    /**
     * Determine whether the given ISD object can be converted into a
     * sensor model state string.
     *
     * \param[in] imageSupportData  The image support data object
     * \param[in] sensorModelName   The sensor model name
     * \param[out] convertible      True if the state string can be constructed from the given parameters, otherwise false.
     */
    PLUGIN_API(canISDBeConvertedToSensorModelState)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             bool& convertible) THROWS;

    /**
     * Convert the given ISD object into a sensor model state string.
     *
     * \param[in] imageSupportData  The image support data object
     * \param[in] sensorModelName   The sensor model name
     * \param[out] sensorModelState     The sensor model state string
     */
    PLUGIN_API(convertISDToSensorModelState)
            (const tsm_ISD& imageSupportData,
             const std::string& sensorModelName,
             std::string& sensorModelState) THROWS;

    /**
     * Get plugin name.
     *
     * \param[out] pluginName   The name of the plugin
     */
    PLUGIN_API(getPluginName)(std::string& pluginName) THROWS;

private:
    //typedef std::list<TSMSensorModel*> SIDDModels;

    // This special constructor is responsible for registering this plugin by
    // invoking the special "int" base class constructor. Since this is
    // private, only a member of this class, e.g. theRegisteringObject, can
    // invoke it.
    SIDDPlugin();

    static SIDD_EXPORT_API const char PLUGIN_NAME[];
    static SIDD_EXPORT_API const char SENSOR_MODEL_NAME[];
    static SIDD_EXPORT_API const char MANUFACTURER[];
    static SIDD_EXPORT_API const char RELEASE_DATE[];
    static SIDD_EXPORT_API const int VERSION;

    static const SIDDPlugin mPlugin;
};

}

#endif
