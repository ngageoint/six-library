/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2025, Arka Group, L.P.
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

#ifndef __SIX_CSM_SIX_PLUGIN_H__
#define __SIX_CSM_SIX_PLUGIN_H__

/**
 * @class SIXPlugin
 *
 * @brief This plugin implements the CSM Plugin API for SICD data
 *
 * @author Martina Schultz
 */

#include "Plugin.h"

#ifdef _WIN32
# ifdef SIX_CSM_LIBRARY
#  define SIX_CSM_EXPORT_API __declspec(dllexport)
# else
#  define SIX_CSM_EXPORT_API __declspec(dllimport)
# endif
#else
#  define SIX_CSM_EXPORT_API
#endif

namespace six
{
namespace CSM
{
class SIXPlugin: public csm::Plugin
{
public:
    /**
     * This method only exists to create a public interface to SIXPlugin because
     * no objects can be constructed and therefore no methods can be called on
     * this class.  If statically linking SIXPlugin into a program, this method
     * may need to be called so that the linker does not discard SIXPlugin.
     */
    static void _publicInterface()
    {
        (void)mPlugin.getPluginName();
    }

    /**
     * Get plugin name.
     *
     * \return The name of the plugin
     */
    virtual std::string getPluginName() const;

    /**
     * Get manufacturer name.
     *
     * \return The name of the plugin manufacturer
     */
    virtual std::string getManufacturer() const;

    /**
     * Get release date.
     *
     * \return releaseDate The release date of the plugin
     */
    virtual std::string getReleaseDate() const;

    /**
     * Get CSM version
     *
     * \return CSM API version that the plugin conforms to
     */
    virtual csm::Version getCsmVersion() const;

    /**
     * Get number of sensor models.
     *
     * \return The number of sensor models supported by the plugin (1)
     */
    virtual size_t getNumModels() const;

    /**
     * Get the name of the sensor model referenced by the supplied index.
     *
     * \param[in] modelIndex  The index of the sensor model
     *
     * \return The sensor model name
     */
    virtual std::string getModelName(size_t modelIndex) const;

    /**
     * Get the family of the sensor model referenced by the supplied index.
     *
     * \param[in] modelIndex  The index of the sensor model
     *
     * \return The sensor model family
     */
    virtual std::string getModelFamily(size_t modelIndex) const;

    /**
     * Get the version of the sensor model identified by the supplied name.
     *
     * \param[in] modelName  The sensor model name
     *
     * \return The sensor model version
     */
    virtual csm::Version getModelVersion(const std::string& modelName) const;

    /**
     * Determine whether the sensor model identified by the supplied name can
     * be constructed from the given state string.
     *
     * \param[in]  modelName   The sensor model name
     * \param[in]  modelState  The sensor model state string
     * \param[out] warnings    Unused
     *
     * \return True if the sensor model can be constructed from the given
     * parameters, otherwise false.
     */
    virtual bool canModelBeConstructedFromState(
       const std::string& modelName,
       const std::string& modelState,
       csm::WarningList* warnings) const;

    /**
     * Determine whether the sensor model identified by the supplied name can
     * be constructed from the given ISD object.
     *
     * \param[in]  imageSupportData  The image support data object
     * \param[in]  modelName         The sensor model name
     * \param[out] warnings          Unused
     *
     * \return True if the sensor model can be constructed from the given
     * parameters, otherwise false.
     */
    virtual bool canModelBeConstructedFromISD(
       const csm::Isd& imageSupportData,
       const std::string& modelName,
       csm::WarningList* warnings) const;

    /**
     * Construct the sensor model from the given state string.
     *
     * \param[in]  modelState  The sensor model state string
     * \param[out] warnings    Unused
     *
     * \return The sensor model object
     */
    virtual SIX_CSM_EXPORT_API csm::Model* constructModelFromState(
       const std::string& modelState,
       csm::WarningList* warnings) const;

    /**
     * Construct the sensor model from the given ISD object.
     *
     * \param[in]  imageSupportData  The image support data object
     * \param[in]  modelName         The sensor model name
     * \param[out] warnings          Unused
     *
     * \return The sensor model object
     */
    virtual SIX_CSM_EXPORT_API csm::Model* constructModelFromISD(
       const csm::Isd& imageSupportData,
       const std::string& modelName,
       csm::WarningList* warnings) const;

    /**
     * Obtain the sensor model name from the given state string.
     *
     * \param[in]  modelState  The sensor model state string
     * \param[out] warnings    Unused
     *
     * \return The sensor model name
     */
    virtual std::string getModelNameFromModelState(
       const std::string& modelState,
       csm::WarningList* warnings) const;

    /**
     * Determine whether the given ISD object can be converted into a
     * sensor model state string.
     *
     * \param[in]  imageSupportData  The image support data object
     * \param[in]  modelName         The sensor model name
     * \param[out] warnings          Unused
     *
     * \return True if the state string can be constructed from the given
     * parameters, otherwise false.
     */
    virtual bool canISDBeConvertedToModelState(
       const csm::Isd& imageSupportData,
       const std::string& modelName,
       csm::WarningList* warnings) const;

    /**
     * Convert the given ISD object into a sensor model state string.
     *
     * \param[in]  imageSupportData  The image support data object
     * \param[in]  modelName         The sensor model name
     * \param[out] warnings          Unused
     *
     * \return The sensor model state string
     */
    virtual std::string convertISDToModelState(
       const csm::Isd& imageSupportData,
       const std::string& modelName,
       csm::WarningList* warnings) const;

private:
    // This special constructor is responsible for registering this plugin by
    // invoking the special "int" base class constructor. Since this is
    // private, only a member of this class, e.g. theRegisteringObject, can
    // invoke it.
    SIXPlugin();

    static SIX_CSM_EXPORT_API const char PLUGIN_NAME[];
    static SIX_CSM_EXPORT_API const char MANUFACTURER[];
    static SIX_CSM_EXPORT_API const char RELEASE_DATE[];

    static const SIXPlugin mPlugin;

    static const size_t SICD_MODEL_INDEX = 0;
    static const size_t SIDD_MODEL_INDEX = 1;
};
}
}

#endif
