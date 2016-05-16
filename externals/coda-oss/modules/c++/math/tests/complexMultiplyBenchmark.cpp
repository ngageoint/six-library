/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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

/* Users guide

    To use this test, you must run the executable with 3 extra numbers
    the first number is the size of the initial array
    the second number is the growth factor, each time the size is increased,
        the old size is multiplied by this number
    the third number is the number of times the size should grow
    the fourth number (optional) denotes whether to loop or not, a 1 indicates
        to loop

    examples:
    ./complexBenchmark 10 2 15
        --this will run the test for array sizes 10, 20, 40,... until the array
            size has been increased 15 times

    ./complexBenchmark 10 2 15 1
        --runs the same as above, but conserves memory by looping to simulate
            increasing the size of the array

*/

/*  Results:
        When using the non looping benchmark, aka for large continuous data 
    sets, it was approximately the same for both methods 
*/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <complex>
#include <vector>
#include <sys/StopWatch.h>
#include <str/Convert.h>

const size_t NUM_TRIALS = 4;
const size_t MAX_SIZE = 10E9;

//Limits num so that it is less than 100
template<typename T>
void limit(T & num)
{
    while (100 < num)
    {
        num /= 10;
    }
}


/*
 *  \purpose
 *      Determine the mean of the vector in, using complex addition
 *      Time how long the operation takes
 *
 *  \params
 *      wtch: a sys::RealTimeStopWatch that performs the timing operations
 *      in: the vector whose mean is being found
 *      sze: the size of the vector
 *      numLoops: allows for use with looping benchmark, specifies loop amount
 *
 *  \output
 *      duration: The amount of time the operation took according to wtch
 *      return value: The mean of the vector
 */
void getMultWComplex(sys::RealTimeStopWatch& wtch,
                     const std::vector<std::complex<float> >& in,
                     size_t sze,
                     double& duration,
                     size_t numLoops = 1)
{
    //declare starting point
    std::vector<std::complex<float> > out (sze, 0);
    double dblFactor = 1.23486;
    float factor = dblFactor;
    double incFact = .0000001;
    //start the watch
    wtch.start();

    //find the mean
    for(size_t j = 0; j < numLoops; ++j)
    {
        for (size_t i = 0; i < sze; ++i)
        {
            out[i] = in[i] *  factor;
            dblFactor += incFact;
            factor = dblFactor;
        }
    }

    //stop the watch and record the duration
    duration = wtch.stop();
}

/*
 *  \purpose
 *      Determine the mean of the vector in, using addition of real
 *          and imaginary parts separately
 *      Time how long the operation takes
 *
 *  \params
 *      wtch: a sys::RealTimeStopWatch that performs the timing operations
 *      in: the vector whose mean is being found
 *      sze: the size of the vector
 *      numLoops: allows for use with looping benchmark, specifies loop amount
 *
 *  \output
 *      duration: The amount of time the operation took according to wtch
 *      return value: The mean of the vector
 */
void getMultWDouble(sys::RealTimeStopWatch& wtch,
                    const std::vector<std::complex<float> >& in,
                    size_t sze,
                    double& duration,
                    size_t numLoops = 1)
{
    //declare starting values
    float I = 0.0;
    float Q = 0.0;
    std::vector<std::complex<float> > out (sze, 0);
    double dblFactor = 1.23486;
    float factor = dblFactor;
    double incFact = .0000001;

    //start the watch
    wtch.start();

    //find the mean
    for (size_t j = 0; j < numLoops; ++j)
    {
        for (size_t i = 0; i < sze; ++i)
        {
            I = in[i].real() * factor;
            Q = in[i].imag() * factor;

            out[i] = std::complex<float>(I, Q);
            dblFactor += incFact;
            factor = dblFactor;
        }
    }

    //stop the watch and record the duration;
    duration = wtch.stop();


}

//Prints out the results in a table format
void print(std::ostream& out, size_t sze, double durOne, double durTwo)
{
    std::cout << std::setw(15) << sze
        << std::setw(15) << durOne/1000
        << std::setw(25) << durTwo/1000 << std::endl;
}


/*
 *  \purpose
 *      Run the looping versioin of the benchmark
 *
 *  \params
 *      size: the size of the vector
 *      growthFactor: factor that the vector's size is simulated to grow by
 *          with each iteration
 *      numGrowths: The number of growths before ending the simulation
 *
 *  \output
 *      out: an ostringstream that stores the output from the simulations
 */
void loopingBenchmark(size_t size,
                      size_t growthFactor,
                      size_t numGrowths,
                      std::ostringstream& out)
{
    //declare the vector
    std::vector<std::complex<float> > arr(size);

    //fill the vector based on a random number
    srand(time(NULL));

    float real = rand() % 100 + 1;
    float imag = rand() % 100 + 1;

    arr[0] = std::complex<float>(real, imag);
    for (size_t i = 0; i < size; ++i)
    {
        real += arr[i-1].real() + arr[i-1].imag();
        imag += arr[i-1].imag() * arr[i-1].real();

        limit(real);
        limit(imag);

        arr[i] = std::complex<float>(real, imag);
    }

    //run the simulation
    size_t numLoops = 1;
    for (size_t i = 0; i < numGrowths; ++i)
    {
        sys::RealTimeStopWatch cmplxWatch;
        sys::RealTimeStopWatch dblWatch;

        double cmplxTime;
        double dblTime;

        for(size_t k = 0; k < NUM_TRIALS; ++k)
        {
            //find the complex mean
            getMultWComplex(cmplxWatch, arr, size, cmplxTime, numLoops);

            //find the mean using doubles
            getMultWDouble(dblWatch, arr, size, dblTime, numLoops);

            //output the results
            print(out, size * numLoops, cmplxTime, dblTime);
        }
        //simulate vector size growth
        numLoops *= growthFactor;

        //return if growth simulated would be too large to handle
        if (sizeof(std::complex<float>) * size * numLoops > MAX_SIZE)
        {
            std::cout << "ending early to prevent growth spiraling" << std::endl;
            return;
        }

    }
}

//determines how large the vector will actually grow for preallocation
size_t decideSize(size_t initSize, size_t growthFactor, size_t numGrowths)
{
    //setup size calculation variables
    const size_t MAX_SZE = MAX_SIZE / (sizeof( std::complex<float>));
    size_t largestPosGrowth =
        initSize * std::pow(static_cast<double>(growthFactor),
                            static_cast<double>(numGrowths));
    size_t largestPosSize = std::min(largestPosGrowth, MAX_SZE);

    //if growth is too high, find last growth less than MaxSize
    if (largestPosSize == MAX_SZE)
    {
        //simulate scaling until scaling would exceed MAX_SIZE
        while (largestPosSize * growthFactor < MAX_SZE)
        {
            largestPosSize *= growthFactor;
        }
    }

    return largestPosSize;
}

/*
 *  \purpose
 *      Run the singlePass version of the benchmark
 *
 *  \params
 *      size: the size of the vector
 *      growthFactor: factor that the vector's size grows by with each iteration
 *      numGrowths: The number of growths before ending the simulation
 *
 *  \output
 *      out: an ostringstream that stores the output from the simulations
 */
void singlePassBenchmark(size_t size,
                         size_t growthFactor,
                         size_t numGrowths,
                         std::ostringstream& out)
{
    //Determine end size of Vector
    size_t endSize = decideSize(size, growthFactor, numGrowths);

    //Initialize the vector ahead of time and then fill it
    std::vector<std::complex<float> > arr;
    arr.reserve(endSize);

    srand(time(NULL));

    float real =  rand() % 100 + 1;
    float imag =  rand() % 100 + 1;

    arr.push_back(std::complex<float>(real, imag));

    for (size_t j = 1; j < endSize; ++j)
    {
        real += (arr[j-1].real() + arr[j-1].imag());
        imag += (arr[j-1].imag() * arr[j-1].real());

        //limit the range of the variables
        limit(real);
        limit(imag);

        arr.push_back(std::complex<float>(real, imag));
    }

    //run the simulation
    for (size_t i = 0; i < numGrowths; ++i)
    {

        sys::RealTimeStopWatch cmplxWatch;
        sys::RealTimeStopWatch dblWatch;

        double cmplxTime;
        double dblTime;
        for (size_t k = 0; k < NUM_TRIALS; ++k)
        {
            //find the mena using complex values
            getMultWComplex(cmplxWatch, arr, size, cmplxTime);
            
            //find the mean using doubles
            getMultWDouble(dblWatch, arr, size, dblTime);

            //output the results
            print(out, size, cmplxTime, dblTime);
        }

        //increase size of vector
        size *= growthFactor;

        //return if growth gets too large
        if (sizeof(std::complex<float>) * size > 10E10)
        {
            std::cout << "ending early to prevent growth spiraling" << std::endl;
            return;
        }
    }

}

int main(int argc, char** argv)
{
    if (argc != 4 && argc != 5)
    {
        std::cerr << "ERROR, incorrect calling" << std::endl;
        std::cerr << "use ./test <InitialArraySize>  <growthFactor>"
                  << " <numberOfIterations> [Loop?]"
                  << std::endl
                  << "a 1 in the Loop? spot means use looping benchmark"
                  << ".  The looping benchmark changes the behavior and"
                  << " can change the results, but allows for less memory usage"
                  << std::endl;
        return 1;
    }

    //read in commandline arguements
    size_t sze = str::toType<size_t>(argv[1]);
    size_t growthFactor = str::toType<size_t>(argv[2]);
    size_t numIter = str::toType<size_t>(argv[3]);
    size_t toLoop = 0;
    if (argc == 5)
    {
        toLoop = str::toType<size_t>(argv[4]);
    }

    //setup ostringstream
    std::ostringstream out;
    out << std::setprecision(5);
    out.setf(std::ios::fixed);
    out.setf(std::ios::left);

    //run the appropriate simulation
    if (toLoop == 1)
    {
        loopingBenchmark(sze, growthFactor, numIter, out);
    }
    else
    {
        singlePassBenchmark(sze, growthFactor, numIter, out);
    }

    //output the results
    std::cout << out.str() << std::endl;
}
