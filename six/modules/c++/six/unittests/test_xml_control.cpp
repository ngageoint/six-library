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

#include <std/filesystem>

#include <six/XMLControl.h>
#include <string>
#include <vector>
#include "TestCase.h"

namespace fs = std::filesystem;

// prefer SIX_DEFAULT_SCHEMA_PATH, existing scripts use DEFAULT_SCHEMA_PATH
#if defined(DEFAULT_SCHEMA_PATH) && !defined(SIX_DEFAULT_SCHEMA_PATH)
#define SIX_DEFAULT_SCHEMA_PATH DEFAULT_SCHEMA_PATH 
#endif
#ifndef SIX_DEFAULT_SCHEMA_PATH
// Don't want to set a dummy schema path to a directory that exists as that causes
// the code to check for valid schemas and validate.
#if defined(_WIN32)
#define SIX_DEFAULT_SCHEMA_PATH R"(C:\some\path)" // just to compile ...
#else
#define SIX_DEFAULT_SCHEMA_PATH R"(/some/path)" // just to compile ...
#endif
#endif

TEST_CASE(loadCompiledSchemaPath)
{
    sys::OS().unsetEnv("SIX_SCHEMA_PATH");
    std::vector<std::string> schemaPaths;
    six::XMLControl::loadSchemaPaths(schemaPaths);

    size_t schemaPathsSize = 0;
    if (fs::exists(SIX_DEFAULT_SCHEMA_PATH))
    {
        schemaPathsSize++;
    }
    TEST_ASSERT_EQ(schemaPaths.size(), schemaPathsSize);
    if (!schemaPaths.empty())
    {
        TEST_ASSERT_EQ(schemaPaths[0], SIX_DEFAULT_SCHEMA_PATH);
    }
}

TEST_CASE(respectGivenPaths)
{
    std::vector<std::string> schemaPaths = {"some/path"};
    sys::OS().setEnv("SIX_SCHEMA_PATH", "another/path", true /*overwrite*/);
    six::XMLControl::loadSchemaPaths(schemaPaths);
    TEST_ASSERT_EQ(schemaPaths.size(), static_cast<size_t>(1));
}

TEST_CASE(loadFromEnvVariable)
{
    std::vector<std::string> schemaPaths;
    sys::OS().setEnv("SIX_SCHEMA_PATH", "another/path", true /*overwrite*/);
    six::XMLControl::loadSchemaPaths(schemaPaths);
    TEST_ASSERT_EQ(schemaPaths.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(schemaPaths[0], "another/path");
}

TEST_CASE(ignoreEmptyEnvVariable)
{
    std::vector<std::string> schemaPaths;
    sys::OS().setEnv("SIX_SCHEMA_PATH", "   ", true /*overwrite*/);
    six::XMLControl::loadSchemaPaths(schemaPaths);

    size_t schemaPathsSize = 0;
    if (fs::exists(SIX_DEFAULT_SCHEMA_PATH))
    {
        schemaPathsSize++;
    }
    TEST_ASSERT_EQ(schemaPaths.size(), schemaPathsSize);
    if (!schemaPaths.empty())
    {
        TEST_ASSERT_EQ(schemaPaths[0], SIX_DEFAULT_SCHEMA_PATH);
    }
}

TEST_MAIN((void)argv; (void)argc;
    TEST_CHECK(loadCompiledSchemaPath);
    TEST_CHECK(respectGivenPaths);
    TEST_CHECK(loadFromEnvVariable);
    TEST_CHECK(ignoreEmptyEnvVariable);
    )
