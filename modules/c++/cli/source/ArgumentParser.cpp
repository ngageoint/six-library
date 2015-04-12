/* =========================================================================
 * This file is part of cli-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include "cli/ArgumentParser.h"
#include <iterator>

#define _MAX_ARG_LINE_LEN 21

namespace cli
{
void _writeArgumentHelp(std::ostream& out, const std::string heading,
                        size_t maxFlagsWidth,
                        const std::vector<std::string>& flags,
                        const std::vector<std::string>& helps)
{
    std::ostringstream s;
    out << heading << std::endl;
    for (size_t i = 0, num = flags.size(); i < num; ++i)
    {
        s.str("");
        std::string flag = flags[i];
        std::string help = helps[i];
        if (flag.size() <= maxFlagsWidth)
        {
            s << "  ";
            s.width(maxFlagsWidth + 1);
            s << std::left << flag;
            s.width(0);
            s << help;
            out << s.str() << std::endl;
        }
        else
        {
            s << "  ";
            s << flag;
            out << s.str() << std::endl;
            s.str("");
            s.width(maxFlagsWidth + 3);
            s << " ";
            s.width(0);
            s << help;
            out << s.str() << std::endl;
        }
    }
}
}

cli::ArgumentParser::ArgumentParser() :
    mHelpEnabled(true), mPrefixChar('-')
{
}

cli::ArgumentParser::~ArgumentParser()
{
    for (std::vector<cli::Argument*>::iterator it = mArgs.begin(); it
            != mArgs.end(); ++it)
        if (*it)
            delete *it;
}

/**
 * Shortcut for adding an argument
 */
cli::Argument* cli::ArgumentParser::addArgument(std::string nameOrFlags,
                                                std::string help,
                                                cli::Action action,
                                                std::string dest,
                                                std::string metavar,
                                                int minArgs, int maxArgs,
                                                bool required)
{
    cli::Argument *arg = new cli::Argument(nameOrFlags, this);

    if (arg->isPositional())
    {
        action = cli::STORE;
        // if it is positional and a min args is supplied, require it
        if (minArgs > 0)
            required = true;
    }

    switch (action)
    {
    case cli::STORE:
    case cli::SUB_OPTIONS:
        if (minArgs < 0)
        {
            minArgs = 1;
            maxArgs = 1;
        }
        break;
    case cli::STORE_TRUE:
    case cli::STORE_FALSE:
    case cli::STORE_CONST:
    case cli::VERSION:
        minArgs = 1;
        maxArgs = 1;
        break;
    }

    arg->setMinArgs(minArgs);
    arg->setMaxArgs(maxArgs);
    arg->setAction(action);
    arg->setDestination(dest);
    arg->setHelp(help);
    arg->setMetavar(metavar);
    arg->setRequired(required);
    mArgs.push_back(arg);
    return arg;
}

cli::ArgumentParser& cli::ArgumentParser::setDescription(const std::string d)
{
    mDescription = d;
    return *this;
}

cli::ArgumentParser& cli::ArgumentParser::setProlog(const std::string prolog)
{
    mProlog = prolog;
    return *this;
}

cli::ArgumentParser& cli::ArgumentParser::setEpilog(const std::string epilog)
{
    mEpilog = epilog;
    return *this;
}

cli::ArgumentParser& cli::ArgumentParser::setUsage(const std::string usage)
{
    mUsage = usage;
    return *this;
}

cli::ArgumentParser& cli::ArgumentParser::enableHelp(bool flag)
{
    mHelpEnabled = flag;
    return *this;
}

cli::ArgumentParser& cli::ArgumentParser::setProgram(const std::string program)
{
    mProgram = program;
    return *this;
}

void cli::ArgumentParser::printHelp(std::ostream& out, bool andExit) const
{
    std::vector<std::string> posFlags, opFlags, posHelps, opHelps, opUsage,
            posUsage;
    size_t maxFlagsWidth = 0;

    processFlags(posFlags, opFlags, posHelps, opHelps, opUsage, posUsage,
                 maxFlagsWidth);

    if (!mProlog.empty())
        out << mProlog << std::endl << std::endl;

    out << "usage: ";
    if (mUsage.empty())
    {
        std::string progName = mProgram;
        out << (progName.empty() ? "program" : progName);
        if (!opUsage.empty())
            out << " " << str::join(opUsage, " ");
        if (!posUsage.empty())
            out << " " << str::join(posUsage, " ");
        out << std::endl;
    }
    else
        out << mUsage << std::endl;

    if (!mDescription.empty())
        out << std::endl << mDescription << std::endl;

    if (posFlags.size() > 0)
    {
        out << std::endl;
        cli::_writeArgumentHelp(out, "positional arguments:", maxFlagsWidth,
                                posFlags, posHelps);
    }

    if (opFlags.size() > 0)
    {
        out << std::endl;
        cli::_writeArgumentHelp(out, "optional arguments:", maxFlagsWidth,
                                opFlags, opHelps);
    }

    if (!mEpilog.empty())
        out << std::endl << mEpilog << std::endl;

    if (andExit)
        exit(cli::EXIT_USAGE);
}

cli::Results* cli::ArgumentParser::parse(int argc, const char** argv)
{
    if (mProgram.empty() && argc > 0)
        setProgram(std::string(argv[0]));
    std::vector < std::string > args;
    for (int i = 1; i < argc; ++i)
        args.push_back(std::string(argv[i]));
    return parse(args);
}
cli::Results* cli::ArgumentParser::parse(const std::vector<std::string>& args)
{
    if (mProgram.empty())
        setProgram("program");

    std::map<std::string, Argument*> shortFlags;
    std::map<std::string, Argument*> longFlags;
    std::map<std::string, Argument*> shortOptionsFlags;
    std::map<std::string, Argument*> longOptionsFlags;
    std::vector<Argument*> positionalArgs;

    // first, validate the arguments
    for (std::vector<cli::Argument*>::const_iterator it = mArgs.begin(); it
            != mArgs.end(); ++it)
    {
        cli::Argument *arg = *it;
        std::string argVar = arg->getVariable();

        if (arg->isPositional())
        {
            positionalArgs.push_back(arg);
        }
        else
        {
            const std::vector<std::string>& argShortFlags =
                    arg->getShortFlags();
            const std::vector<std::string>& argLongFlags = arg->getLongFlags();
            bool subOption = (arg->getAction() == cli::SUB_OPTIONS);
            for (std::vector<std::string>::const_iterator it =
                    argShortFlags.begin(); it != argShortFlags.end(); ++it)
            {
                std::string op = *it;
                std::map<std::string, Argument*>& flagMap =
                        (subOption ? shortOptionsFlags : shortFlags);
                if (flagMap.find(op) != flagMap.end())
                    parseError(FmtX("Conflicting option: %c%s", mPrefixChar,
                                    op.c_str()));
                flagMap[op] = arg;
            }
            for (std::vector<std::string>::const_iterator it =
                    argLongFlags.begin(); it != argLongFlags.end(); ++it)
            {
                std::string op = *it;
                std::map<std::string, Argument*>& flagMap =
                        (subOption ? longOptionsFlags : longFlags);
                if (flagMap.find(op) != flagMap.end())
                    parseError(FmtX("Conflicting option: %c%c%s", mPrefixChar,
                                    mPrefixChar, op.c_str()));
                flagMap[op] = arg;
            }
        }

    }

    std::vector < std::string > explodedArgs;
    // next, check for combined short options
    for (size_t i = 0, s = args.size(); i < s; ++i)
    {
        std::string argStr = args[i];
        if (argStr.size() > 1 && argStr[0] == mPrefixChar && argStr[1]
                != mPrefixChar)
        {
            std::string flag = argStr.substr(1);
            if (shortFlags.find(flag) != shortFlags.end())
            {
                explodedArgs.push_back(argStr);
            }
            else
            {
                // check for =
                if (argStr.find("=") != std::string::npos)
                {
                    std::vector < std::string > parts = str::split(argStr, "=",
                                                                   2);
                    std::copy(parts.begin(), parts.end(),
                              std::back_inserter(explodedArgs));
                }
                else
                {
                    // first, see if it is an extra option
                    std::vector < std::string > parts = str::split(argStr, ":",
                                                                   2);
                    if (parts.size() == 2)
                    {
                        explodedArgs.push_back(argStr);
                    }
                    else
                    {
                        // split up each char as separate options
                        // only the last will get any additional args
                        for (size_t j = 0, n = flag.size(); j < n; ++j)
                        {
                            std::string charFlag = flag.substr(j, 1);
                            std::ostringstream oss;
                            oss << mPrefixChar << charFlag;
                            explodedArgs.push_back(oss.str());
                        }
                    }
                }
            }
        }
        else if (argStr.size() > 2 && argStr[0] == mPrefixChar && argStr[1]
                == mPrefixChar)
        {
            // check for =
            if (argStr.find("=") != std::string::npos)
            {
                std::vector < std::string > parts = str::split(argStr, "=", 2);
                std::copy(parts.begin(), parts.end(),
                          std::back_inserter(explodedArgs));
            }
            else
                explodedArgs.push_back(argStr);
        }
        else
        {
            explodedArgs.push_back(argStr);
        }
    }

    cli::Results *results = new Results;
    cli::Results *currentResults = NULL;
    for (size_t i = 0, s = explodedArgs.size(); i < s; ++i)
    {
        currentResults = results; // set the pointer
        std::string argStr = explodedArgs[i];
        cli::Argument *arg = NULL;
        std::string optionsStr("");
        if (argStr.size() > 2 && argStr[0] == mPrefixChar && argStr[1]
                == mPrefixChar)
        {
            std::string flag = argStr.substr(2);
            if (longFlags.find(flag) != longFlags.end())
            {
                arg = longFlags[flag];
            }
            else if (mHelpEnabled && flag == "help")
            {
                printHelp(std::cerr, true);
            }
            else
            {
                // check if it's an options flag
                std::vector < std::string > parts = str::split(flag, ":", 2);
                if (parts.size() == 2 && longOptionsFlags.find(parts[0])
                        != longOptionsFlags.end())
                {
                    arg = longOptionsFlags[parts[0]];
                    optionsStr = parts[1];
                    std::string argVar = arg->getVariable();
                    if (!results->hasSubResults(argVar))
                        results->put(argVar, new cli::Results);
                    currentResults = results->getSubResults(argVar);
                }
                else
                {
                    throw except::Exception(Ctxt(FmtX("Invalid option: [%s]",
                                                      argStr.c_str())));
                }
            }
        }
        else if (argStr.size() > 1 && argStr[0] == mPrefixChar && argStr[1]
                != mPrefixChar)
        {
            std::string flag = argStr.substr(1);
            if (shortFlags.find(flag) != shortFlags.end())
            {
                arg = shortFlags[flag];
            }
            else if (mHelpEnabled && flag == "h")
            {
                printHelp(std::cerr, true);
            }
            else
            {
                // check if it's an options flag
                std::vector < std::string > parts = str::split(flag, ":", 2);
                if (parts.size() == 2 && shortOptionsFlags.find(parts[0])
                        != shortOptionsFlags.end())
                {
                    arg = shortOptionsFlags[parts[0]];
                    optionsStr = parts[1];
                    std::string argVar = arg->getVariable();
                    if (!results->hasSubResults(argVar))
                        results->put(argVar, new cli::Results);
                    currentResults = results->getSubResults(argVar);
                }
                else
                {
                    throw except::Exception(Ctxt(FmtX("Invalid option: [%s]",
                                                      argStr.c_str())));
                }
            }
        }

        if (arg != NULL)
        {
            std::string argVar = arg->getVariable();
            switch (arg->getAction())
            {
            case cli::STORE:
            {
                cli::Value
                        *v =
                                currentResults->hasValue(argVar) ? currentResults->getValue(
                                                                                            argVar)
                                                                 : new cli::Value;
                int maxArgs = arg->getMaxArgs();
                // risky, I know...
                bool added = false;
                while (i < s - 1)
                {
                    std::string nextArg(explodedArgs[i + 1]);
                    if (nextArg.size() > 1 && nextArg[0] == mPrefixChar)
                    {
                        // it's another flag, so we break out
                        break;
                    }
                    if (maxArgs >= 0 && v->size() >= maxArgs)
                    {
                        // it's another positional argument, so we break out
                        break;
                    }
                    v->add(nextArg);
                    ++i;
                    added = true;
                }

                if (!added)
                    parseError(
                               FmtX(
                                    "option requires value or has exceeded its max: [%s]",
                                    argVar.c_str()));

                currentResults->put(argVar, v);
                break;
            }
            case cli::STORE_TRUE:
                currentResults->put(argVar, new cli::Value(true));
                break;
            case cli::STORE_FALSE:
                currentResults->put(argVar, new cli::Value(false));
                break;
            case cli::STORE_CONST:
            {
                const Value* constVal = arg->getConst();
                currentResults->put(argVar, constVal ? constVal->clone() : NULL);
                break;
            }
            case cli::SUB_OPTIONS:
            {
                if (optionsStr.empty())
                    parseError(FmtX("invalid sub option: [%s]", argVar.c_str()));
                cli::Value
                        *v =
                                currentResults->hasValue(optionsStr) ? currentResults->getValue(
                                                                                                optionsStr)
                                                                     : new cli::Value;
                if (i < s - 1)
                {
                    std::string nextArg = explodedArgs[i + 1];
                    if (nextArg.size() > 1 && nextArg[0] == mPrefixChar)
                    {
                        // it's another flag
                        // this indicates the sub op is a bool
                        v->add(true);
                    }
                    else
                    {
                        v->add(nextArg);
                        ++i;
                    }
                }
                else
                {
                    // this indicates the sub op is a bool
                    v->add(true);
                }
                currentResults->put(optionsStr, v);
                break;
            }
            case cli::VERSION:
                //TODO
                break;
            }
        }
        else
        {
            // it's a positional argument
            cli::Value *lastPosVal = NULL;
            for (std::vector<cli::Argument*>::iterator it =
                    positionalArgs.begin(); it != positionalArgs.end(); ++it)
            {
                cli::Argument *posArg = *it;
                std::string argVar = posArg->getVariable();
                int maxArgs = posArg->getMaxArgs();
                if (currentResults->hasValue(argVar))
                {
                    cli::Value *posVal = lastPosVal
                            = currentResults->getValue(argVar);
                    if (posVal->size() >= maxArgs)
                        continue;
                    break;
                }
                else if (maxArgs != 0)
                {
                    lastPosVal = new cli::Value;
                    currentResults->put(argVar, lastPosVal);
                    break;
                }
            }
            if (lastPosVal)
                lastPosVal->add(argStr);
            else
                parseError("too many arguments");
        }
    }

    // add the defaults
    for (std::vector<cli::Argument*>::const_iterator it = mArgs.begin(); it
            != mArgs.end(); ++it)
    {
        cli::Argument *arg = *it;
        std::string argMeta = arg->getMetavar();
        std::string argVar = arg->getVariable();
        std::string argId = arg->isPositional() && !argMeta.empty() ? argMeta
                                                                    : argVar;

        if (!results->hasValue(argVar))
        {
            const Value* defaultVal = arg->getDefault();
            if (defaultVal != NULL)
                results->put(argVar, defaultVal->clone());
            else if (arg->getAction() == cli::STORE_FALSE)
                results->put(argVar, new cli::Value(true));
            else if (arg->getAction() == cli::STORE_TRUE)
                results->put(argVar, new cli::Value(false));
            else if (arg->isRequired())
                parseError(FmtX("missing required argument: [%s]",
                                argVar.c_str()));
        }


        // validate the argument value against the choices
        // TODO: add option to make case sensitive
        std::vector<std::string> choices = arg->getChoices();
        if (!choices.empty())
        {
            bool isValid = false;
            std::string val = results->getValue(argVar)->toString();
            str::lower(val);

            for (int i = 0; i < choices.size(); i++)
            {
                std::string choice = choices[i];
                str::lower(choice);
                if (str::containsOnly(val, choice))
                {
                    isValid = true;
                    break;
                }
            }
            if (!isValid)
            {
                parseError(FmtX("invalid option for [%s]", argVar.c_str()));
            }
        }


        // validate # of args
        int minArgs = arg->getMinArgs();
        int maxArgs = arg->getMaxArgs();
        size_t numGiven =
                results->hasValue(argVar) ? results->getValue(argVar)->size()
                                          : 0;

        if (arg->isRequired() || numGiven > 0)
        {
            if (minArgs > 0 && numGiven < minArgs)
                parseError(FmtX("not enough arguments, %d required: [%s]",
                                minArgs, argId.c_str()));
            if (maxArgs >= 0 && numGiven > maxArgs)
                parseError(FmtX("too many arguments, %d supported: [%s]",
                                maxArgs, argId.c_str()));
        }
    }

    return results;
}

void cli::ArgumentParser::printUsage(std::ostream& out, bool andExit,
                                     const std::string message) const
{
    out << "usage: ";
    if (mUsage.empty())
    {
        std::vector<std::string> posFlags, opFlags, posHelps, opHelps, opUsage,
                posUsage;
        size_t maxFlagsWidth = 0;

        processFlags(posFlags, opFlags, posHelps, opHelps, opUsage, posUsage,
                     maxFlagsWidth);

        std::string progName = mProgram;
        out << (progName.empty() ? "program" : progName);
        if (!opUsage.empty())
            out << " " << str::join(opUsage, " ");
        if (!posUsage.empty())
            out << " " << str::join(posUsage, " ");
    }
    else
        out << mUsage;
    if (!message.empty())
        out << std::endl << std::endl << message;
    out << std::endl;
    if (andExit)
        exit(cli::EXIT_USAGE);
}

void cli::ArgumentParser::parseError(const std::string& msg)
{
    std::ostringstream s;
    s << "usage: ";
    if (mUsage.empty())
    {
        std::vector<std::string> posFlags, opFlags, posHelps, opHelps, opUsage,
                posUsage;
        size_t maxFlagsWidth = 0;

        processFlags(posFlags, opFlags, posHelps, opHelps, opUsage, posUsage,
                     maxFlagsWidth);

        std::string progName = mProgram;
        s << (progName.empty() ? "program" : progName);
        if (!opUsage.empty())
            s << " " << str::join(opUsage, " ");
        if (!posUsage.empty())
            s << " " << str::join(posUsage, " ");
    }
    else
        s << mUsage;
    s << "\n" << msg;
    throw except::ParseException(s.str());
}

void cli::ArgumentParser::processFlags(std::vector<std::string>& posFlags,
                                       std::vector<std::string>& opFlags,
                                       std::vector<std::string>&posHelps,
                                       std::vector<std::string>&opHelps,
                                       std::vector<std::string>&opUsage,
                                       std::vector<std::string>&posUsage,
                                       size_t& maxFlagsWidth) const
{
    std::ostringstream s;

    if (mHelpEnabled)
    {
        std::string helpMsg = FmtX("%ch, %c%chelp", mPrefixChar, mPrefixChar,
                                   mPrefixChar);
        maxFlagsWidth = std::max<size_t>(helpMsg.size(), maxFlagsWidth);
        opFlags.push_back(helpMsg);
        opHelps.push_back("show this help message and exit");
    }

    for (std::vector<cli::Argument*>::const_iterator it = mArgs.begin(); it
            != mArgs.end(); ++it)
    {
        cli::Argument *arg = *it;
        const std::string& argName = arg->getName();
        const cli::Action& argAction = arg->getAction();
        const std::vector<std::string>& argChoices = arg->getChoices();
        const std::string& argMetavar = arg->getMetavar();
        const std::string& argHelp = arg->getHelp();
        const cli::Value* defaultVal = arg->getDefault();

        s.str("");
        s << argHelp;
        if (defaultVal)
            s << " (default: " << defaultVal->toString() << ")";
        std::string helpMsg = s.str();

        s.str("");
        if (!argMetavar.empty())
            s << argMetavar;
        else if (!argChoices.empty())
            s << "{" << str::join(argChoices, ",") << "}";
        std::string meta = s.str();

        if (arg->isPositional())
        {
            //positional argument
            std::string op = meta.empty() ? argName : meta;
            maxFlagsWidth = std::max<size_t>(op.size(), maxFlagsWidth);
            posFlags.push_back(op);
            if (arg->showsHelp())
            {
                posHelps.push_back(helpMsg);
                posUsage.push_back(op);
            }
        }
        else
        {
            std::vector < std::string > ops;
            const std::vector<std::string>& argShortFlags =
                    arg->getShortFlags();
            const std::vector<std::string>& argLongFlags = arg->getLongFlags();
            for (size_t i = 0, n = argShortFlags.size(); i < n; ++i)
            {
                s.str("");
                s << mPrefixChar << argShortFlags[i];
                if (argAction == cli::SUB_OPTIONS)
                {
                    if (meta.empty())
                        s << ":ARG[=VALUE]";
                    else
                        s << ":" << meta;
                }
                else if (!meta.empty())
                    s << " " << meta;
                ops.push_back(s.str());
            }
            for (size_t i = 0, n = argLongFlags.size(); i < n; ++i)
            {
                s.str("");
                s << mPrefixChar << mPrefixChar << argLongFlags[i];
                if (argAction == cli::SUB_OPTIONS)
                {
                    if (meta.empty())
                        s << ":ARG[=VALUE]";
                    else
                        s << ":" << meta;
                }
                else if (!meta.empty())
                    s << " " << meta;
                ops.push_back(s.str());
            }
            if (!ops.empty())
            {
                s.str("");
                s << "[" << ops[0] << "]";
                if (arg->showsHelp())
                    opUsage.push_back(s.str());

                std::string opMsg = str::join(ops, ", ");
                maxFlagsWidth = std::max<size_t>(opMsg.size(), maxFlagsWidth);
                if (arg->showsHelp())
                {
                    opFlags.push_back(opMsg);
                    opHelps.push_back(helpMsg);
                }
            }
        }
    }
    maxFlagsWidth = std::min<size_t>(maxFlagsWidth, _MAX_ARG_LINE_LEN);
}

