/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *    
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *      
 * mem-c++ is free software; you can redistribute it and/or modify
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

#include <mem/ScratchMemory.h>
#include <mem/BufferView.h>
#include <cli/ArgumentParser.h>
#include <str/Convert.h>
#include <vector>
#include <fstream>
#include <string>

namespace
{
struct Operation
{
    Operation(const std::string& op_,
              const std::string& name_,
              const size_t bytes_) :
        op(op_),
        name(name_),
        bytes(bytes_)
    {
    }

    std::string op;
    std::string name;
    size_t bytes;
    mem::BufferView<sys::ubyte> buffer;
};

/*!
 * \class Visualizer
 *
 * \brief Handles the inclusion and visualization of a put or release operation
 */
class Visualizer
{
public:
    /*!
     * Constructor
     *
     * \param prevOperations All operations handled in previous iterations
     * \param iteration Which iteration is being processed
     * \param[in,out] htmlFile Ofstream to .html file
     * \param[in,out] cssFile Ofstream to .css file
     */
    Visualizer(const std::vector<Operation>& prevOperations,
               size_t iteration,
               std::ofstream& htmlFile,
               std::ofstream& cssFile):
        mPrevOperations(prevOperations),
        mIteration(iteration),
        mHTMLFile(htmlFile),
        mCSSFile(cssFile)
    {
        mStartPtr = NULL;
        mColors.push_back("lightgrey");
        mColors.push_back("lightblue");
        mColors.push_back("lightcyan");
        mReleasedColors.push_back("indianred");
        mReleasedColors.push_back("coral");
        mReleasedColors.push_back("salmon");
    }

    /*!
     * Find the lowest start address, which dictates the first block, and set mStartPtr.
     *
     * \param operations A vector of all operations
     */
    void setStartPtr(const std::vector<Operation>& operations)
    {
        mStartPtr = operations.at(0).buffer.data;
        for (size_t ii = 0; ii < operations.size(); ++ii)
        {
            if (operations.at(ii).buffer.data - mStartPtr <= 0)
            {
                mStartPtr = operations.at(ii).buffer.data;
            }
        }
    }

    /*!
     * Write the CSS and HTML to create a box for a single operation.
     *
     * \param op Operation to draw a box of
     * \param color_iter Iterator to arbitrarily select a color
     */
    void createBox(const Operation& op, size_t colorIter)
    {
        std::string color = mColors.at(colorIter % 3);
        std::string height = "25px;\n";
        if (op.op == "release")
        {
            color = mReleasedColors.at(colorIter % 3);
            height = "30px;\n";
        }

        mCSSFile << "#" << op.name << " { \n";
        mCSSFile << "height: " << height;
        mCSSFile << "display: inline-block;\n";
        mCSSFile << "position: absolute;\n";
        mCSSFile << "background-color: " << color << ";\n";
        mCSSFile << "width: " << op.buffer.size << "px;\n";
        mCSSFile << "margin-left: " << op.buffer.data - mStartPtr << "px;\n";
        mCSSFile << "border-style: solid;\n";
        mCSSFile << "border-width: 1px;\n";
        mCSSFile << "}\n";

        mHTMLFile << "<div id=\"" << op.name << "\">&nbsp;" << op.name[0] << "</div>\n";
    }

    /*!
     * When building a new visualizer, all operations that have already been done
     * in previous iterations must be recreated.
     *
     * \param[out] currentOperations Operations for this iteration
     * \param[out] scratch Scratch memory object
     */
    void handlePrevOps(std::vector<Operation>& currentOperations, mem::ScratchMemory& scratch)
    {
        for (size_t ii = 0; ii < mPrevOperations.size(); ++ii)
        {
            const std::string segmentName = std::string(1, mPrevOperations.at(ii).name[0]) +
                    str::toString(mIteration);

            if (mPrevOperations.at(ii).op == "put")
            {
                scratch.put<sys::ubyte>(segmentName, mPrevOperations.at(ii).bytes, 1, 1);
                currentOperations.push_back(Operation("put",
                                                      segmentName,
                                                      mPrevOperations.at(ii).bytes));
            }
            else if (mPrevOperations.at(ii).op == "release")
            {
                scratch.release(segmentName);
                currentOperations.push_back(Operation("release",
                                                      segmentName,
                                                      mPrevOperations.at(ii).bytes));
            }
        }
    }

    /*!
     * Runs the randomly generated test case.
     *
     * \param[in,out] currentOperations Operations for this iteration
     * \param[in,out] bufferName The name/key of the new segment
     * \param[in,out] notReleasedKeys The keys that have not been released
     * \param[out] usedBufferSpace How much memory has already been used up
     * \param[out] scratch Scratch memory object
     */
    void randomTest(std::vector<Operation>& currentOperations,
                    unsigned char& bufferName,
                    std::vector<unsigned char>& notReleasedKeys,
                    mem::ScratchMemory& scratch)
    {
        handlePrevOps(currentOperations, scratch);

        const size_t numElements = (rand() % 150) + 20;
        unsigned int releaseIfThree = (rand() % 3) + 1;

        if ((releaseIfThree == 3) && (currentOperations.size() > 1) && !notReleasedKeys.empty())
        {
            unsigned int keyToReleaseIndex = (rand() % notReleasedKeys.size());
            const std::string segmentName = std::string(1, notReleasedKeys.at(keyToReleaseIndex)) +
                    str::toString(mIteration);

            scratch.release(segmentName);
            notReleasedKeys.erase(notReleasedKeys.begin() + keyToReleaseIndex);

            currentOperations.push_back(Operation("release", segmentName, numElements));
        }
        else
        {
            const std::string segmentName = std::string(1, bufferName) + str::toString(mIteration);
            scratch.put<sys::ubyte>(segmentName, numElements, 1, 1);
            currentOperations.push_back(Operation("put", segmentName, numElements));
            notReleasedKeys.push_back(bufferName);

            ++bufferName;
        }
    }

    /*!
     * Runs a version of the unittest in which concurrent keys are released in a very specific order
     *
     * \param[in,out] currentOperations Operations for this iteration
     * \param[in,out] bufferName The name/key of the new segment
     * \param[in,out] notReleasedKeys The keys that have not been released
     * \param[in,out] testIter Used to keep track of which step of the unittest to do
     * \param[out] usedBufferSpace How much memory has already been used up
     * \param[out] scratch Scratch memory object
     */
    void concurrentBlockTest(std::vector<Operation>& currentOperations,
                             unsigned char& bufferName,
                             size_t& testIter,
                             mem::ScratchMemory& scratch)
    {
        handlePrevOps(currentOperations, scratch);
        size_t numElements = (rand() % 150) + 20;

        // Put operations
        if (testIter == 0 || testIter == 1 || testIter == 2 || testIter == 5)
        {
            const std::string segmentName = std::string(1, bufferName) +
                    str::toString(mIteration);
            scratch.put<sys::ubyte>(segmentName, numElements, 1, 1);
            currentOperations.push_back(Operation("put", segmentName, numElements));
            ++testIter;
            ++bufferName;
        }
        else if (testIter == 3)
        {
            const std::string segmentName = std::string(1, bufferName - 2) +
                    str::toString(mIteration);
            scratch.release(segmentName);
            currentOperations.push_back(Operation("release", segmentName, numElements));
            ++testIter;
        }
        else if (testIter == 4)
        {
            const std::string segmentName = std::string(1, bufferName - 1) +
                    str::toString(mIteration);
            scratch.release(segmentName);
            currentOperations.push_back(Operation("release", segmentName, numElements));
            ++testIter;
        }
        else if (testIter == 6)
        {
            const std::string segmentName = std::string(1, bufferName - 4) +
                    str::toString(mIteration);
            scratch.release(segmentName);
            currentOperations.push_back(Operation("release", segmentName, numElements));
            testIter = 0;
        }
    }

    /*!
     * Runs a version of the unittest in which connected keys are released in a very specific order
     *
     * \param[in,out] currentOperations Operations for this iteration
     * \param[in,out] bufferName The name/key of the new segment
     * \param[in,out] notReleasedKeys The keys that have not been released
     * \param[in,out] testIter Used to keep track of which step of the unittest to do
     * \param[out] usedBufferSpace How much memory has already been used up
     * \param[out] scratch Scratch memory object
     */
    void connectedBlockTest(std::vector<Operation>& currentOperations,
                             unsigned char& bufferName,
                             size_t& testIter,
                             mem::ScratchMemory& scratch)
    {
        handlePrevOps(currentOperations, scratch);
        size_t numElements = (rand() % 150) + 20;

        if (testIter == 0 || testIter == 1 || testIter == 3 || testIter == 5)
        {
            const std::string segmentName = std::string(1, bufferName) +
                    str::toString(mIteration);
            scratch.put<sys::ubyte>(segmentName, numElements, 1, 1);
            currentOperations.push_back(Operation("put", segmentName, numElements));
            ++testIter;
            ++bufferName;
        }
        else if (testIter == 2)
        {
            const std::string segmentName = std::string(1, bufferName - 1) +
                    str::toString(mIteration);
            scratch.release(segmentName);
            currentOperations.push_back(Operation("release", segmentName, numElements));
            ++testIter;
        }
        else if (testIter == 4)
        {
            const std::string segmentName = std::string(1, bufferName - 3) +
                    str::toString(mIteration);
            scratch.release(segmentName);
            currentOperations.push_back(Operation("release", segmentName, numElements));
            ++testIter;
        }
        else if (testIter == 6)
        {
            const std::string segmentName = std::string(1, bufferName - 2) +
                    str::toString(mIteration);
            scratch.release(segmentName);
            currentOperations.push_back(Operation("release", segmentName, numElements));
            testIter = 0;
        }
    }

private:
    std::vector<std::string> mColors;
    std::vector<std::string> mReleasedColors;
    std::vector<Operation> mPrevOperations;
    sys::ubyte* mStartPtr;
    size_t mIteration;

    std::ofstream& mHTMLFile;
    std::ofstream& mCSSFile;
};
}

int main(int argc, char** argv)
{
    cli::ArgumentParser parser;

    parser.setDescription("Software to visualize scratch memory test cases in HTML/CSS");
    parser.addArgument("--test", "Select which test case to run", cli::STORE, "test")->setDefault("random");

    const cli::Results* options(parser.parse(argc, argv));
    const std::string testType(options->get<std::string>("test"));

    srand((unsigned)time(nullptr));

    std::ofstream htmlFile;
    htmlFile.open("scratch_release.html");
    std::ofstream cssFile;
    cssFile.open("style.css");

    //Headers
    htmlFile << "<!DOCTYPE HTML>\n";
    htmlFile << "<html>\n";
    htmlFile << "<head>\n";
    htmlFile << "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">\n";
    htmlFile << "</head>\n";
    htmlFile << "<body>\n";

    std::vector<Operation> prevOperations;
    unsigned char bufferName = 'a';

    size_t testIter = 0;

    std::vector<unsigned char> notReleasedKeys;

    for (size_t jj = 0; jj < 20; ++jj)
    {
        std::vector<Operation> currentOperations;
        mem::ScratchMemory scratch;
        Visualizer visualize(prevOperations, jj, htmlFile, cssFile);

        if (testType == "random")
        {   visualize.randomTest(currentOperations,
                                 bufferName,
                                 notReleasedKeys,
                                 scratch);
        }
        else if (testType == "concurrent")
        {
            visualize.concurrentBlockTest(currentOperations,
                                          bufferName,
                                          testIter,
                                          scratch);
        }
        else if (testType == "connected")
        {
            visualize.connectedBlockTest(currentOperations,
                                         bufferName,
                                         testIter,
                                         scratch);
        }
        else
        {
            std::cout << "--test must be \"random\", \"concurrent\", or \"connected\"\n";
            return 1;
        }

        scratch.setup();

        //This draws the line. Draw one per scratch instance
        htmlFile << "<hr><br>\n";
        cssFile << "hr { \n";
        cssFile << "height: 1px;\n";
        cssFile << "float: left;\n";
        cssFile << "width: " << scratch.getNumBytes() << "px;\n";
        cssFile << "position: absolute;\n";
        cssFile << "}\n";

        //Goes through each buffer in order
        for (size_t ii = 0; ii < currentOperations.size(); ++ii)
        {
            currentOperations.at(ii).buffer =
                    scratch.getBufferView<sys::ubyte>(currentOperations.at(ii).name);
        }

        visualize.setStartPtr(currentOperations);

        for (size_t ii = 0; ii < currentOperations.size(); ++ii)
        {
                visualize.createBox(currentOperations.at(ii), ii);
        }

        htmlFile << "<br><br><br>\n";
        prevOperations = currentOperations;
    }


    htmlFile << "</body>\n";
    htmlFile << "</html>\n";

    htmlFile.close();
    cssFile.close();

    try
    {
        int result = system("firefox scratch_release.html");
        if (result > 0) { /*fix compiler warning*/ }
    }
    catch(const except::Exception&)
    {
        std::cout << "Failed to open html file in firefox\n";
    }

    return 0;
}
