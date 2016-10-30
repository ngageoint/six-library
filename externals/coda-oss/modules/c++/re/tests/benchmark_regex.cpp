/* =========================================================================
 * This file is part of re-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * re-c++ is free software; you can redistribute it and/or modify
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


#include <string>
#include <fstream>

#include <import/re.h>
#include <import/sys.h>
#include <import/str.h>
#include <import/except.h>

// First just benchmark the Regex creation time (including compile)
double BM_RegexCreation(uint64_t numIterations, const std::string& regexString)
{
    sys::RealTimeStopWatch sw;

    sw.start();
    for (uint64_t ii = 0; ii < numIterations; ++ii)
    {
        re::Regex regex(regexString);
    }
    double elapsedTimeMS = sw.stop();

    return elapsedTimeMS / numIterations;
}


// Now benchmark the actual string-matching
double BM_RegexMatch(uint64_t numIterations, const std::string& fileString, const std::string& regexString)
{
    sys::RealTimeStopWatch sw;
    re::Regex regex(regexString);

    sw.start();
    for (uint64_t ii = 0; ii < numIterations; ++ii)
    {
        regex.matches( fileString );
    }
    double elapsedTimeMS = sw.stop();

    return elapsedTimeMS / numIterations;
}


int main(int argc, char** argv)
{
    try
    {
        std::string regexString("beam(Id|String)");

        if (argc < 3)
        {
            std::cerr << "Usage: " << sys::Path::basename(argv[0])
                      << " inputFile numIterations [regexString]" << std::endl;
            return 1;
        }
    
        sys::Uint64_T numIterations = str::toType<sys::Uint64_T>(argv[2]);

        // Grab the regex string if provided at the command line
        if (argc > 3)
        {
            regexString = argv[3];
        }

        // Print the current timestamp for reference
        sys::LocalDateTime ldt;
        std::cout << ldt.format(std::string("%Y-%m-%d %H:%M:%S")) << std::endl;
    
        // Open our text file and feed it into the static buffer
        std::ifstream bigFin(argv[1]);
        if (!bigFin.is_open())
        {
            std::cerr << "Error opening text file!" << std::endl;
            return 2;
        }
    
        size_t size = bigFin.tellg();
        std::vector<char> fileVec(size, '\0');
    
        bigFin.seekg(0);
        bigFin.read(&fileVec[0], size);
        bigFin.close();

        std::string fileString(&fileVec[0], size);
    
        // Now run the benchmarks
        double swtime0 = BM_RegexCreation(numIterations, regexString);
        double swtime1 = BM_RegexMatch(numIterations, fileString, regexString);

        // Convert ms to ns
        swtime0 *= 1.e6;
        swtime1 *= 1.e6;

        // Pretty-print our results
        std::cout << std::setw(20) << std::left << "Benchmark" << " "
                  << std::setw(20) << std::right << "Time/Iteration (ns)" << " "
                  << std::setw(15) << std::right << "Iterations" << std::endl;
    
        std::cout << std::string(57, '-') << std::endl;
    
        std::cout << std::setw(20) << std::left << "BM_RegexCreation" << " "
                  << std::setw(20) << std::right << std::fixed << std::setprecision(0) << swtime0 << " "
                  << std::setw(15) << std::right << numIterations << std::endl;

        std::cout << std::setw(20) << std::left << "BM_RegexMatch" << " "
                  << std::setw(20) << std::right << std::fixed << std::setprecision(0) << swtime1 << " "
                  << std::setw(15) << std::right << numIterations << std::endl;
    }
    catch (except::Exception ex)
    {
        std::cerr << "An exception occurred!" << std::endl;
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (std::exception ex)
    {
        std::cerr << "An exception occurred!" << std::endl;
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    
    return 0;
}

