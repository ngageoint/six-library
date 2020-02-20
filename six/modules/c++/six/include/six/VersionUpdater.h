/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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
#ifndef __SIX_VERSION_UPDATER_H__
#define __SIX_VERSION_UPDATER_H__

#include <string>
#include <vector>
#include <logging/Logger.h>
#include <six/Data.h>

namespace six
{
/*!
 * \class VersionUpdater
 * Responsible for updating metadata to a valid state
 * for a new version.
 * This is intended to be used quick prototyping work for standards
 * updates. Some inserted fields will be valid XML but
 * mathematically garbage.
 */
class VersionUpdater
{
public:
    /*!
     * Constructor
     * \param[in,out] data Metadata to update
     * \param targetVersion Version to update `data` to.
     *                      Must be greater than current version of `data`.
     * \param versions All the allowable version strings for this type of metadata
     * \param[out] log Warnings will be emitting for fields that were populated
     *                 with either bad data or guesses
     * \throws If `targetVersion` is invalid
     */
    VersionUpdater(Data& data,
                   const std::string& targetVersion,
                   const std::vector<std::string>& versions,
                   logging::Logger& log);

    virtual ~VersionUpdater()
    {
    }

    //! Update version of data to target version
    void update();

protected:
    /*!
     * Subclasses will override this method to update
     * metadata to the next valid version.
     * The metadata's version is guaranteed to be valid,
     * and less than the target version.
     * The actual version number should not change
     * during this function.
     */
    virtual void updateSingleIncrement() = 0;
    void emitWarning(const std::string& fieldName);

    /*!
     * Add a parameter to the appropriate processing block,
     * indicating that a metadata field has been populated
     * with a guess, or garbage data to force XML validation
     * \param fieldName Name of Metadata field with bad value
     */
    virtual void addProcessingParameter(const std::string& fieldName) = 0;

    /*!
     * Add metadata-appropriate XML block indicating that
     * a version-update processing step was applied.
     */
    virtual void recordProcessingStep() = 0;

    const std::string& mTarget;
    const std::vector<std::string> mVersions;

private:
    bool finished() const;
    void bumpVersion();
    void validateTargetVersion() const;
    void initializeLogging();

    Data& mData;
    logging::Logger& mLog;
    bool mLoggingInitialized;
};
}

#endif
