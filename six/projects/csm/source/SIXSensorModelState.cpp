/* =========================================================================
 * This file is part of the CSM SIX Plugin
 * =========================================================================
 *
 * (C) Copyright 2025, Arka Group, L.P.
 *
 * The CSM SIX Plugin is free software; you can redistribute it and/or modify
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

#include <six/csm/SIXSensorModel.h>

namespace
{
const std::string ADJ_PARAMS_TAG = "ADJ_PARAMS:";
const std::string IID_TAG = "IID:";
const std::string COV_TAG = "COV:";

const std::string NONE_TAG = "NONE:";
const std::string FICTITIOUS_TAG = "FICTITIOUS:";
const std::string REAL_TAG = "REAL:";
const std::string FIXED_TAG = "FIXED:";

bool startsWith(const std::string& string, const std::string& prefix)
{
    return (string.rfind(prefix, 0) == 0);
}

void consumePrefix(std::string& string, const std::string& prefix)
{
    if (!startsWith(string, prefix))
    {
        std::ostringstream ostr;
        ostr << "Invalid sensor model state.  Expecting '" << prefix;
        ostr << "' but found '" << string.substr(0, prefix.length()) << "'.";
        throw csm::Error(csm::Error::INVALID_SENSOR_MODEL_STATE,
                         ostr.str(),
                         "SICDSensorModel::replaceModelStateImpl");
    }
    string = string.substr(prefix.length());
}
}

namespace six
{
namespace CSM
{
SIXSensorModelState::SIXSensorModelState() : mOverrideSensorCovariance(false)
{
    std::fill_n(mAdjustableTypes,
                static_cast<size_t>(scene::AdjustableParams::NUM_PARAMS),
                csm::param::REAL);
    std::fill_n(mAdjustableValues,
                static_cast<size_t>(scene::AdjustableParams::NUM_PARAMS),
                0.0);
}

SIXSensorModelState::SIXSensorModelState(const std::string& stateString,
                                         std::string& remainder)
{
    remainder = initializeFromString(stateString);
}

std::string SIXSensorModelState::initializeFromString(
        const std::string& stateString)
{
    std::string remainder = stateString;

    while (true)
    {
        if (remainder.rfind(ADJ_PARAMS_TAG, 0) == 0)
        {
            remainder = remainder.substr(ADJ_PARAMS_TAG.length());
            for (size_t paramIdx = 0;
                 paramIdx < scene::AdjustableParams::NUM_PARAMS;
                 paramIdx++)
            {
                if (startsWith(remainder, NONE_TAG))
                {
                    remainder = remainder.substr(NONE_TAG.length());
                    mAdjustableTypes[paramIdx] = csm::param::Type::NONE;
                }
                else if (startsWith(remainder, FICTITIOUS_TAG))
                {
                    remainder = remainder.substr(FICTITIOUS_TAG.length());
                    mAdjustableTypes[paramIdx] = csm::param::Type::FICTITIOUS;
                }
                else if (startsWith(remainder, REAL_TAG))
                {
                    remainder = remainder.substr(REAL_TAG.length());
                    mAdjustableTypes[paramIdx] = csm::param::Type::REAL;
                }
                else if (startsWith(remainder, FIXED_TAG))
                {
                    remainder = remainder.substr(FIXED_TAG.length());
                    mAdjustableTypes[paramIdx] = csm::param::Type::FIXED;
                }
                else
                    throw csm::Error(csm::Error::INVALID_SENSOR_MODEL_STATE,
                                     "Invalid sensor model state: unrecognized "
                                     "parameter tag",
                                     "SICDSensorModel::replaceModelStateImpl");

                size_t consumed = 0;
                // TODO: catch std::invalid_argument?
                mAdjustableValues[paramIdx] = std::stod(remainder, &consumed);
                remainder = remainder.substr(consumed);

                consumePrefix(remainder, ",");
            }
            consumePrefix(remainder, " ");
        }
        else if (remainder.rfind(IID_TAG, 0) == 0)
        {
            remainder = remainder.substr(IID_TAG.length());
            size_t consumed = 0;
            // TODO: catch std::invalid_argument?
            size_t nameLen = std::stoul(remainder, &consumed);
            remainder = remainder.substr(consumed);

            consumePrefix(remainder, ":");

            mDatasetName = remainder.substr(0, nameLen);
            remainder = remainder.substr(nameLen);

            consumePrefix(remainder, " ");
        }
        else if (remainder.rfind(COV_TAG, 0) == 0)
        {
            mOverrideSensorCovariance = true;
            remainder = remainder.substr(COV_TAG.length());
            for (size_t idx1 = 0; idx1 < scene::AdjustableParams::NUM_PARAMS;
                 idx1++)
            {
                for (size_t idx2 = 0;
                     idx2 < scene::AdjustableParams::NUM_PARAMS;
                     idx2++)
                {
                    size_t consumed = 0;
                    // TODO: catch std::invalid_argument?
                    mSensorCovariance(idx1, idx2) =
                            std::stod(remainder, &consumed);
                    remainder = remainder.substr(consumed);
                    consumePrefix(remainder, ",");
                }
            }
            consumePrefix(remainder, " ");
        }
        else if (remainder.rfind("<", 0) == 0)
        {
            return remainder;
        }
        else
        {
            throw csm::Error(csm::Error::INVALID_SENSOR_MODEL_STATE,
                             "Invalid sensor model state: unrecognized text",
                             "SICDSensorModel::replaceModelStateImpl");
        }
    }
}

std::string SIXSensorModelState::toString() const
{
    std::string adj_params;
    for (size_t paramIdx = 0; paramIdx < scene::AdjustableParams::NUM_PARAMS;
         paramIdx++)
    {
        switch (mAdjustableTypes[paramIdx])
        {
        case csm::param::Type::NONE:
            adj_params += "NONE:";
            break;
        case csm::param::Type::FICTITIOUS:
            adj_params += "FICTITIOUS:";
            break;
        case csm::param::Type::REAL:
            adj_params += "REAL:";
            break;
        case csm::param::Type::FIXED:
            adj_params += "FIXED:";
            break;
        }
        adj_params += FmtX("%.18g,", mAdjustableValues[paramIdx]);
    }

    std::string cov;
    for (size_t idx1 = 0; idx1 < scene::AdjustableParams::NUM_PARAMS; idx1++)
    {
        for (size_t idx2 = 0; idx2 < scene::AdjustableParams::NUM_PARAMS;
             idx2++)
        {
            cov += FmtX("%.18g,", mSensorCovariance(idx1, idx2));
        }
    }

    return std::string(" ") + ADJ_PARAMS_TAG + adj_params + std::string(" ") +
            IID_TAG + std::to_string(mDatasetName.length()) + std::string(":") +
            mDatasetName + std::string(" ") + COV_TAG + cov;
}

}
}