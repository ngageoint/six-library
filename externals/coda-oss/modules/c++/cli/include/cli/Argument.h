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

#ifndef __CLI_ARGUMENT_H__
#define __CLI_ARGUMENT_H__

#include <import/str.h>
#include "cli/Value.h"

namespace cli
{

enum Action
{
    STORE, STORE_TRUE, STORE_FALSE, STORE_CONST, SUB_OPTIONS, VERSION
};

// forward declaration
class ArgumentParser;

/**
 * An individual argument describes one entity from the command line.
 *
 * TODO: add support for case insensitive or standardizing choices/parsing
 */
class Argument
{
public:

    ~Argument();

    Argument* addFlag(const std::string& flag);
    Argument* setAction(Action action);
    Argument* setMinArgs(int num);
    Argument* setMaxArgs(int num);
    Argument* setDefault(Value* val, bool own = false);
    Argument* setChoices(const std::vector<std::string>& choices);
    Argument* addChoice(const std::string& choice);
    Argument* setHelp(const std::string& help);
    Argument* setMetavar(const std::string& metavar);
    Argument* setDestination(const std::string& dest);
    Argument* setConst(Value* val, bool own = false);
    Argument* setRequired(bool flag);
    Argument* setShowsHelp(bool flag);

    template <typename T>
    Argument* setConst(const T val)
    {
        setConst(new Value(val), true);
        return this;
    }

    template <typename T>
    Argument* setDefault(const T val)
    {
        setDefault(new Value(val), true);
        return this;
    }

    inline const std::string& getName() const { return mName; }
    inline const std::vector<std::string>& getShortFlags() const { return mShortFlags; }
    inline const std::vector<std::string>& getLongFlags() const { return mLongFlags; }
    inline Action getAction() const { return mAction; }
    inline int getMinArgs() const { return mMinArgs; }
    inline int getMaxArgs() const { return mMaxArgs; }
    inline const Value* getDefault() const { return mDefaultValue; }
    inline const std::vector<std::string>& getChoices() const { return mChoices; }
    inline bool isRequired() const { return mRequired; }
    inline const std::string& getHelp() const { return mHelp; }
    inline const std::string& getMetavar() const { return mMetavar; }
    inline const std::string& getDestination() const { return mDestination; }
    inline const Value* getConst() const { return mConstValue; }
    inline bool showsHelp() const { return mShowsHelp; }

    std::string getVariable() const;
    bool isPositional() const;

protected:
    std::string mName;
    std::vector<std::string> mShortFlags;
    std::vector<std::string> mLongFlags;
    Action mAction;
    int mMinArgs;
    int mMaxArgs;
    Value* mDefaultValue;
    bool mOwnDefault;
    std::vector<std::string> mChoices;
    std::string mHelp;
    std::string mMetavar;
    std::string mDestination;
    Value* mConstValue;
    bool mOwnConst;
    bool mRequired;
    bool mShowsHelp;
    ArgumentParser* mParser;

    friend class ArgumentParser;
    Argument(const std::string& nameOrFlags, ArgumentParser* parser);

    std::string validateFlag(const std::string& flag) const;
};

}
#endif
