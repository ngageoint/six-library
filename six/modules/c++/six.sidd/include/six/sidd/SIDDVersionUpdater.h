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
#ifndef __SIX_SIDD_VERSION_UPDATER_H__
#define __SIX_SIDD_VERSION_UPDATER_H__

#include <six/VersionUpdater.h>
#include <six/sidd/DerivedData.h>
#include <vector>
#include <string>

namespace six
{
namespace sidd
{
/*!
 * \class SIDDVersionUpdater
 * Responsible for updating SIDD metadata to a valid state
 * for a new version.
 * This is intended to be used quick prototyping work for standards
 * updates. Some inserted fields will be valid XML but
 * mathematically garbage.
 */
class SIDDVersionUpdater : public VersionUpdater
{
    public:
    /*!
     * Constructor
     * \param[in,out] derivedData SIDD metadata to update
     * \param targetVersion Version to update `data` to.
     *                      Must be greater than current version of `data`.
     * \param[out] log Warnings will be emitting for fields that were populated
     *                 with either bad data or guesses
     * \throws If `targetVersion` is invalid
     */
    SIDDVersionUpdater(DerivedData& derivedData,
                       const std::string& targetVersion,
                       logging::Logger& log);

    /*!
     * Get a collection of valid version strings.
     * \return All the allowable version strings for this type of metadata
     */
    static const std::vector<std::string>& getValidVersions();

private:
    void updateSingleIncrement() override;

    void addProcessingParameter(const std::string& fieldName) override;

    void recordProcessingStep() override;

    DerivedData& mData;

    size_t mProcessingModuleIndex;
};
}
}
#endif
