/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <six/VersionUpdater.h>

namespace six
{
VersionUpdater::VersionUpdater(Data& data,
                               const std::string& targetVersion,
                               const std::vector<std::string>& versions,
                               logging::Logger& log) :
    mTarget(targetVersion),
    mVersions(versions),
    mData(data),
    mLog(log),
    mLoggingInitialized(false)
{
    validateTargetVersion();
}

void VersionUpdater::validateTargetVersion() const
{
    const auto targetIt = std::find(mVersions.begin(), mVersions.end(), mTarget);
    if (targetIt == mVersions.end())
    {
        std::ostringstream msg;
        msg << "Unrecognized version: " << mTarget;
        throw except::Exception(Ctxt(msg));
    }

    const auto currentIt = std::find(mVersions.begin(), mVersions.end(),
                                     mData.getVersion());
    if (std::distance(currentIt, targetIt) <= 0)
    {
        std::ostringstream msg;
        msg << "Target version <" << mTarget << "> must be later than "
            << "current version <" << mData.getVersion() << ">";
        throw except::Exception(Ctxt(msg));
    }
}

bool VersionUpdater::finished() const
{
    return mData.getVersion() == mTarget;
}

void VersionUpdater::bumpVersion()
{
    if (finished())
    {
        throw except::Exception(Ctxt("Can't increment version any futher"));
    }

    auto it = std::find(mVersions.begin(), mVersions.end(), mData.getVersion());
    assert(it != mVersions.end());
    ++it;
    assert(it != mVersions.end());
    mData.setVersion(*it);
}

void VersionUpdater::update()
{
    recordProcessingStep();

    while (!finished())
    {
        updateSingleIncrement();
        bumpVersion();
    }
}

void VersionUpdater::initializeLogging()
{
    mLog.warn("The following fields need to be populated "
              "with meaningful values: ");
    mLoggingInitialized = true;
}

void VersionUpdater::emitWarning(const std::string& fieldName)
{
    if (!mLoggingInitialized)
    {
        initializeLogging();
    }
    std::ostringstream msg;
    msg << "  - " << fieldName;
    mLog.warn(msg.str());

    addProcessingParameter(fieldName);
}
}
