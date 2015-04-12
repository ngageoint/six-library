/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <import/sys.h>
#include <fstream>
#include <iomanip>
#include "sys/StopWatch.h"

using namespace sys;

int main(int argc, char **argv)
{
    try
    {
        sys::OS os;

        std::string fileToFind = os.getCurrentWorkingDirectory()
                + os.getDelimiter() + "blah.txt";

        // Some System info
        std::cout << "Some System Info" << std::endl;
        std::cout << "=============================================="
                << std::endl;
        std::cout << "Host: " << os.getNodeName() << std::endl;
        std::cout << "User path: " << os["PATH"] << std::endl;
        std::cout << "Platform: " << os.getPlatformName() << std::endl;
        std::cout << "The delimiter on this platform: " << os.getDelimiter()
                << std::endl;
        std::cout << "The process id: " << os.getProcessId() << std::endl;
        //std::cout << "The user is: " << os.getUsername() << std::endl;
        /////////////////////////////////////////////
        // File exists check!!!
        if (argc == 2)
        {
            std::cerr << "I will look for " << argv[1] << std::endl;
            fileToFind = argv[1];
        }

        if (os.exists(fileToFind))
        {
            std::cout << "Found file: " << fileToFind << std::endl;
            std::cout << "File is of size: " << os.getSize(fileToFind)
                    << std::endl;
        }
        else
        {
            std::cerr << "Did not find file: " << fileToFind << std::endl;
        }
        //////////////////////////////////////////////////
        //  Test directory functions
        std::string cwd = os.getCurrentWorkingDirectory();
        if (os.isFile(cwd))
        {
            std::cerr << "Shouldnt be here: " << cwd << " is not a file!!"
                    << std::endl;
        }
        else if (os.isDirectory(cwd))
        {
            std::cout << "Found directory: " << cwd << std::endl;
        }
        ///////////////////////////////////////////////////
        // Test temp file stuff
        std::string tempFileName = os.getTempName();
        std::ofstream ofs(tempFileName.c_str());
        if (!ofs.is_open())
            throw except::Exception(FmtX("Could not open file named: %s",
                                         tempFileName.c_str()));
        ofs << "Im writing some crap to this file!" << std::endl;
        ofs.close();
        std::cout << "Created file: " << tempFileName << " with size: "
                << os.getSize(tempFileName) << std::endl;
        os.remove(tempFileName);
        std::cout << "Killed file: " << tempFileName << std::endl;

        if (!os.makeDirectory("TEST_DIRECTORY"))
            std::cout << "failed to create test directory" << std::endl;

        else
        {
            std::cout << "Created test directory: TEST_DIRECTORY" << std::endl;
        }
        if (!os.remove("TEST_DIRECTORY"))
        {
            std::cout << "test directory removal failed:"
                    << sys::Err().toString() << std::endl;
        }
        else
        {
            std::cout << "Killed test directory" << std::endl;
        }

        std::cout << "Time Now: " << time(NULL) << std::endl;
        sys::RealTimeStopWatch sw;
        //std::cout << "CPS: " << CLOCKS_PER_SEC << std::endl;
        //	std::cout << "Clock: " << clock() << std::endl;
        std::cout << "Start: " << sw.start() << std::endl;
        for (int i = 0; i < 1000000000; ++i)
        {
            int x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 1" << std::endl;
        sw.pause();
        for (int i = 0; i < 1000000000; ++i)
        {
            int x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 2" << std::endl;
        //sw.start();
        //sw.clear();
        for (int i = 0; i < 1000000000; ++i)
        {
            int x = 1 * 2 * 3;
        }
        std::cout << "Finish Loop 3" << std::endl;
        std::cout << "Stop: " << std::setprecision(50) << sw.stop()
                << std::endl;
        std::cout << "Time Now: " << time(NULL) << std::endl;
        //	std::cout << "Clock: " << clock() << std::endl;

    }
    catch (except::Throwable& t)
    {
        std::cerr << "Caught throwable: " << t.toString() << std::endl;
        exit( EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "Caught unnamed exception" << std::endl;
    }
    return 0;
}
