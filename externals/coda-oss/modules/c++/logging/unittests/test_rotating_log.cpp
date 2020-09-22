/* =========================================================================
 * This file is part of logging-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * logging-c++ is free software; you can redistribute it and/or modify
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

#include <import/logging.h>
#include "TestCase.h"

void cleanupFiles(std::string base)
{
    // cleanup
    sys::OS os;
    for (size_t i = 0;; ++i)
    {
        std::ostringstream oss;
        oss << base << "." << (i + 1);
        std::string fname(oss.str());
        if (os.isFile(fname))
            os.remove(fname);
        else
            break;
    }
    if (os.isFile(base))
        os.remove(base);
}

TEST_CASE(testRotate)
{
    std::string outFile = "test_rotate.txt";
    size_t maxFiles = 1;

    cleanupFiles( outFile);

    sys::OS os;
    {
        std::unique_ptr<logging::Logger> log(new logging::Logger("test"));
        std::unique_ptr<logging::Formatter>
                formatter(new logging::StandardFormatter("%m"));
        std::unique_ptr<logging::Handler>
                logHandler(new logging::RotatingFileHandler(outFile, 10, maxFiles));

        logHandler->setLevel(logging::LogLevel::LOG_DEBUG);
        logHandler->setFormatter(formatter.release());
        log->addHandler(logHandler.release(), true);

        log->debug("0123456789");
        TEST_ASSERT(os.exists(outFile));
        TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));

        log->debug("1");
        TEST_ASSERT(os.isFile(outFile + ".1"));
    }

    cleanupFiles( outFile);
}

TEST_CASE(testNeverRotate)
{
    std::string outFile = "test_rotate.txt";
    cleanupFiles( outFile);

    sys::OS os;
    {
        std::unique_ptr<logging::Logger> log(new logging::Logger("test"));
        std::unique_ptr<logging::Formatter>
                formatter(new logging::StandardFormatter("%m"));
        std::unique_ptr<logging::Handler>
                logHandler(new logging::RotatingFileHandler(outFile));

        for(size_t i = 0; i < 1024; ++i)
        {
            log->debug("test");
        }
        TEST_ASSERT(os.exists(outFile));
        TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));
    }

    cleanupFiles( outFile);
}

TEST_CASE(testRotateReset)
{
    std::string outFile = "test_rotate.txt";
    cleanupFiles( outFile);

    sys::OS os;
    {
        std::unique_ptr<logging::Logger> log(new logging::Logger("test"));
        std::unique_ptr<logging::Formatter>
                formatter(new logging::StandardFormatter("%m"));
        std::unique_ptr<logging::Handler>
                logHandler(new logging::RotatingFileHandler(outFile, 10));
        log->debug("01234567890");
        TEST_ASSERT(os.exists(outFile));
        TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));

        log->debug("0");
        TEST_ASSERT(os.exists(outFile));
        TEST_ASSERT_FALSE(os.isFile(outFile + ".1"));
    }

    cleanupFiles( outFile);
}

int main(int, char**)
{
    TEST_CHECK( testNeverRotate);
    TEST_CHECK( testRotateReset);
    TEST_CHECK( testRotate);
}
