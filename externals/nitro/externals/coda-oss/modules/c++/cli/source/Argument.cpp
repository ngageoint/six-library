/* =========================================================================
 * This file is part of cli-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cli-c++ is free software; you can redistribute it and/or modify
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

#include "cli/Argument.h"
#include "cli/ArgumentParser.h"
#include <iterator>

cli::Argument::Argument(const std::string& nameOrFlags, cli::ArgumentParser* parser):
    mAction(cli::STORE), mMinArgs(0), mMaxArgs(1), mDefaultValue(NULL),
            mOwnDefault(false), mConstValue(NULL), mOwnConst(false),
            mRequired(false), mShowsHelp(true), mParser(parser)
{
    std::vector < std::string > vars = str::split(nameOrFlags, " ");
    if (vars.size() == 1 && !str::startsWith(vars[0], "-"))
        mName = vars[0];
    else
    {
        for (std::vector<std::string>::iterator it = vars.begin(); it
                != vars.end(); ++it)
        {
            addFlag(*it);
        }
    }
}

cli::Argument::~Argument()
{
    if (mOwnDefault && mDefaultValue)
        delete mDefaultValue;
    if (mOwnConst && mConstValue)
        delete mConstValue;
}

cli::Argument* cli::Argument::addFlag(const std::string& flag)
{
    char p = mParser->mPrefixChar;
    std::string p2 = FmtX("%c%c", p, p);
    if (flag.size() > 2 && str::startsWith(flag, p2) && flag[2] != p)
        mLongFlags.push_back(validateFlag(flag.substr(2)));
    else if (flag.size() > 1 && flag[0] == p && flag[1] != p)
        mShortFlags.push_back(validateFlag(flag.substr(1)));
    return this;
}
std::string cli::Argument::validateFlag(const std::string& flag) const
{
    const static std::string idChars =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";
    std::string firstChar(flag.substr(0, 1));
    std::string rest = flag.substr(1);
    if ((!str::isAlphanumeric(firstChar) && firstChar[0] != '_')
            || (!str::containsOnly(rest, idChars)))
        throw except::Exception(Ctxt("invalid flag"));
    return flag;
}

cli::Argument* cli::Argument::setAction(cli::Action action)
{
    mAction = action;
    if (action == cli::STORE_TRUE || action == cli::STORE_FALSE || action
            == cli::STORE_CONST || action == cli::VERSION)
    {
        // the flag, const or version are stored as the single argument
        setMinArgs(1);
        setMaxArgs(1);
    }
    return this;
}
cli::Argument* cli::Argument::setMinArgs(int num)
{
    mMinArgs = num;
    return this;
}
cli::Argument* cli::Argument::setMaxArgs(int num)
{
    mMaxArgs = num;
    return this;
}
cli::Argument* cli::Argument::setDefault(Value* val, bool own)
{
    mDefaultValue = val;
    mOwnDefault = own;
    return this;
}
cli::Argument* cli::Argument::setChoices(
    const std::vector<std::string>& choices)
{
    mChoices.clear();
    mChoices = choices;
    return this;
}
cli::Argument* cli::Argument::addChoice(const std::string& choice)
{
    mChoices.push_back(choice);
    return this;
}
cli::Argument* cli::Argument::setHelp(const std::string& help)
{
    mHelp = help;
    return this;
}
cli::Argument* cli::Argument::setMetavar(const std::string& metavar)
{
    mMetavar = metavar;
    return this;
}
cli::Argument* cli::Argument::setDestination(const std::string& dest)
{
    mDestination = dest;
    return this;
}
cli::Argument* cli::Argument::setConst(Value* val, bool own)
{
    mConstValue = val;
    mOwnConst = own;
    return this;
}
cli::Argument* cli::Argument::setRequired(bool flag)
{
    mRequired = flag;
    return this;
}
cli::Argument* cli::Argument::setShowsHelp(bool flag)
{
    mShowsHelp = flag;
    return this;
}

std::string cli::Argument::getVariable() const
{
    if (!mDestination.empty())
        return mDestination;
    if (!mName.empty())
        return mName;
    if (!mLongFlags.empty())
        return mLongFlags[0];
    return mShortFlags[0];
}

bool cli::Argument::isPositional() const
{
    return mShortFlags.empty() && mLongFlags.empty();
}
