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

#include <import/cli.h>
#include "TestCase.h"
#include <sstream>

TEST_CASE(testValue)
{
    cli::Value v("data");
    TEST_ASSERT_EQ("data", v.get<std::string>());

    v.set(3.14f);
    TEST_ASSERT_ALMOST_EQ(3.14f, v.get<float>());
    TEST_ASSERT_EQ(3, v.get<int>());

    std::vector<float> floats;
    std::vector<std::string> strings;
    for(int i = 0; i < 10; ++i)
    {
        floats.push_back(10.0f * i);
        strings.push_back(str::toString(i));
    }

    // floats
    v.setContainer(floats);
    for(int i = 0; i < 10; ++i)
    {
        TEST_ASSERT_ALMOST_EQ(v.at<float>(i), 10.0f * i);
    }
    TEST_ASSERT_EQ(v.size(), 10);

    // strings
    v.setContainer(strings);
    for(int i = 0; i < 10; ++i)
    {
        TEST_ASSERT_EQ(v.at<std::string>(i), str::toString(i));
    }
    TEST_ASSERT_EQ(v.size(), 10);
}

TEST_CASE(testChoices)
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-t --type", "Specify a type to use", cli::STORE)->addChoice(
            "type1")->addChoice("type2")->addChoice("type3");
    parser.addArgument("-m --many", "Specify a type to use", cli::STORE, "choices", "CHOICES", 0)->addChoice(
            "type1")->addChoice("type2")->addChoice("type3");
    parser.addArgument("images", "Input images", cli::STORE);
    parser.setDescription("This program is kind of pointless, but have fun!");
    parser.setProlog("========= (c) COPYRIGHT BANNER ========= ");
    parser.setEpilog("And that's the usage of the program!");
    std::ostringstream buf;
    parser.printHelp(buf);

    std::unique_ptr<cli::Results> results(parser.parse(str::split("-v", " ")));
    TEST_ASSERT(results->hasValue("verbose"));
    TEST_ASSERT(results->get<bool>("verbose", 0));

    results.reset(parser.parse(str::split("", " ")));
    TEST_ASSERT_EQ(results->get<bool>("verbose", 0), false);

    results.reset(parser.parse(str::split("-t type2", " ")));
    TEST_ASSERT_EQ(results->get<std::string>("type", 0), std::string("type2"));

    try
    {
        results.reset(parser.parse(str::split("-t type2 -t type1", " ")));
        TEST_FAIL("Shouldn't allow multiple types");
    }
    catch(except::Exception&)
    {
    }
    results.reset(parser.parse(str::split("-t type2", " ")));

    results.reset(parser.parse(str::split("-m type2 --many type1 -m type3", " ")));
}

TEST_CASE(testMultiple)
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose --loud -l", "Toggle verbose", cli::STORE_TRUE);

    std::unique_ptr<cli::Results> results(parser.parse(str::split("-v")));
    TEST_ASSERT(results->hasValue("verbose"));
    TEST_ASSERT(results->get<bool>("verbose"));

    results.reset(parser.parse(str::split("-l")));
    TEST_ASSERT(results->get<bool>("verbose"));
    results.reset(parser.parse(str::split("--loud")));
    TEST_ASSERT(results->get<bool>("verbose"));
    results.reset(parser.parse(str::split("")));
    TEST_ASSERT_FALSE(results->get<bool>("verbose"));
}

TEST_CASE(testSubOptions)
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-c --config", "Specify a config file", cli::STORE);
    parser.addArgument("-x --extra", "Extra options", cli::SUB_OPTIONS);
    parser.addArgument("-c --config", "Config options", cli::SUB_OPTIONS);
    std::ostringstream buf;
    parser.printHelp(buf);

    std::unique_ptr<cli::Results> results(parser.parse(str::split("-x:special")));
    TEST_ASSERT(results->hasSubResults("extra"));
    TEST_ASSERT(results->getSubResults("extra")->get<bool>("special"));

    results.reset(parser.parse(str::split("--extra:arg=something -x:arg2 1")));
    TEST_ASSERT(results->hasSubResults("extra"));
    TEST_ASSERT_EQ(results->getSubResults("extra")->get<std::string>("arg"), "something");
    TEST_ASSERT_EQ(results->getSubResults("extra")->get<int>("arg2"), 1);

    results.reset(parser.parse(str::split("--config /path/to/file --config:flag1 -c:flag2=true --config:flag3 false")));
    TEST_ASSERT_EQ(results->get<std::string>("config"), "/path/to/file");
    TEST_ASSERT(results->hasSubResults("config"));
    TEST_ASSERT(results->getSubResults("config")->get<bool>("flag1"));
    TEST_ASSERT(results->getSubResults("config")->get<bool>("flag2"));
    TEST_ASSERT_FALSE(results->getSubResults("config")->get<bool>("flag3"));
}

TEST_CASE(testIterate)
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-c --config", "Specify a config file", cli::STORE);

    std::unique_ptr<cli::Results>
            results(parser.parse(str::split("-v -c config.xml")));
    std::vector<std::string> keys;
    for(cli::Results::const_iterator it = results->begin(); it != results->end(); ++it)
        keys.push_back(it->first);
    TEST_ASSERT_EQ(keys.size(), 2);
    // std::map returns keys in alphabetical order...
    TEST_ASSERT_EQ(keys[0], "config");
    TEST_ASSERT_EQ(keys[1], "verbose");
}

TEST_CASE(testRequired)
{
    cli::ArgumentParser parser;
    parser.setProgram("tester");
    parser.addArgument("-v --verbose", "Toggle verbose", cli::STORE_TRUE);
    parser.addArgument("-c --config", "Specify a config file", cli::STORE)->setRequired(true);

    std::unique_ptr<cli::Results> results;
    TEST_EXCEPTION(results.reset(parser.parse(str::split(""))));
    TEST_EXCEPTION(results.reset(parser.parse(str::split("-c"))));
    results.reset(parser.parse(str::split("-c configFile")));
    TEST_ASSERT_EQ(results->get<std::string>("config"), "configFile");
}

int main(int, char**)
{
    TEST_CHECK( testValue);
    TEST_CHECK( testChoices);
    TEST_CHECK( testMultiple);
    TEST_CHECK( testSubOptions);
    TEST_CHECK( testIterate);
    TEST_CHECK( testRequired);
}
