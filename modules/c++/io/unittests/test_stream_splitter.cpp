/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <algorithm>
#include <string>
#include <vector>
#include <io/StreamSplitter.h>
#include <io/StringStream.h>
#include <TestCase.h>

// return true if the string sequences are the same, false otherwise
bool compareStringSequence(const std::vector<std::string>& a,
                           const std::vector<std::string>& b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    return std::equal(a.begin(), a.end(), b.begin());
}

std::string getTestLine(size_t length)
{
    std::string lineTemplate = "012345678901234567890";
    return lineTemplate.substr(0, length);
}

// join lines into StringStream and verify StreamSplitter produces the same
// sequence of lines
// return true for success, false for failure
bool streamSplitterTestRunner(size_t numLines,
                              size_t lineLength,
                              const std::string& delimiter,
                              size_t bufferSize)
{
    std::vector<std::string> inputLines;
    io::StringStream stream;

    std::string lineString;
    if (lineLength <= 20)
    {
        lineString = getTestLine(lineLength);
    }
    inputLines.push_back(lineString);
    stream.write(lineString.data(), lineString.length());
    size_t numBytesTotal = lineString.length();
    std::vector<size_t> numBytesCumulative;
    if (numLines > 1)
    {
        numBytesTotal += delimiter.length();
    }
    numBytesCumulative.push_back(numBytesTotal);
    for (size_t ii = 1; ii < numLines; ++ii)
    {
        lineString = getTestLine((lineLength <= 20) ? lineLength : ii % 20);
        inputLines.push_back(lineString);
        stream.write(delimiter.data(), delimiter.length());
        stream.write(lineString.data(), lineString.length());
        numBytesTotal += lineString.length();
        if (ii < numLines - 1)
        {
            numBytesTotal += delimiter.length();
        }
        numBytesCumulative.push_back(numBytesTotal);
    }

    io::StreamSplitter splitter(stream, delimiter, bufferSize);
    std::vector<std::string> outputLines;
    std::string substring;
    size_t numBytesReturned = 0;

    if (splitter.getNumSubstringsReturned() != 0 ||
        splitter.getNumBytesReturned() != 0 ||
        splitter.getNumBytesProcessed() != 0)
    {
        return false;
    }

    while (splitter.getNext(substring))
    {
        outputLines.push_back(substring);

        if (splitter.getNumSubstringsReturned() != outputLines.size())
        {
            return false;
        }

        numBytesReturned += substring.size();
        if (numBytesReturned != splitter.getNumBytesReturned() ||
            (numBytesCumulative[outputLines.size() - 1] !=
             splitter.getNumBytesProcessed()))
        {
            return false;
        }
    }

    if (splitter.getNumBytesProcessed() != numBytesTotal)
    {
        return false;
    }

    if (splitter.getNext(substring))
    {
        // stream should be empty
        return false;
    }
    return compareStringSequence(inputLines, outputLines);
}

TEST_CASE(testStreamSplitter)
{
    std::vector<size_t> lineCounts;
    lineCounts.push_back(1);
    lineCounts.push_back(2);
    lineCounts.push_back(3);
    lineCounts.push_back(11);
    lineCounts.push_back(19);
    lineCounts.push_back(20);
    lineCounts.push_back(21);
    lineCounts.push_back(37);

    std::vector<size_t> lineLengths;
    lineLengths.push_back(0);
    lineLengths.push_back(1);
    lineLengths.push_back(9);
    lineLengths.push_back(10);
    lineLengths.push_back(11);
    lineLengths.push_back(50);  // variable line lengths

    std::vector<std::string> delimiters;
    delimiters.push_back(",");
    delimiters.push_back(" ");
    delimiters.push_back("  ");
    delimiters.push_back("aaa");
    delimiters.push_back("abc");
    delimiters.push_back("\n");
    delimiters.push_back("\r\n");

    std::vector<size_t> bufferSizes;
    bufferSizes.push_back(65536); // default size

    bufferSizes.push_back(30); // too small to read full stream at once
    bufferSizes.push_back(31);
    bufferSizes.push_back(32);
    bufferSizes.push_back(33);
    bufferSizes.push_back(34);
    bufferSizes.push_back(35);
    bufferSizes.push_back(36);
    bufferSizes.push_back(37);
    bufferSizes.push_back(38);

    bufferSizes.push_back(7); // too small to fit a single line
    bufferSizes.push_back(8);
    bufferSizes.push_back(9);
    bufferSizes.push_back(10);
    bufferSizes.push_back(11);
    bufferSizes.push_back(12);

    // check every combination of lineCount, lineLength, delimiter, and bufferSize
    for (size_t i_lineCount = 0; i_lineCount < lineCounts.size(); ++i_lineCount)
    {
        const size_t lineCount = lineCounts[i_lineCount];
        for (size_t i_lineLength = 0; i_lineLength < lineLengths.size(); ++i_lineLength)
        {
            const size_t lineLength = lineLengths[i_lineLength];
            for (size_t i_delimiter = 0; i_delimiter < delimiters.size(); ++i_delimiter)
            {
                const std::string delimiter = delimiters[i_delimiter];
                for (size_t i_bufferSize = 0; i_bufferSize < bufferSizes.size(); ++i_bufferSize)
                {
                    const size_t bufferSize = bufferSizes[i_bufferSize];
                    TEST_ASSERT(streamSplitterTestRunner(lineCount, lineLength, delimiter, bufferSize));
                }
            }
        }
    }

}

TEST_CASE(testStreamSplitterEmpty)
{
    // empty stream should return 1 empty token
    std::string substring;
    io::StringStream stream;
    io::StreamSplitter splitter(stream);
    TEST_ASSERT(splitter.getNumSubstringsReturned() == 0);
    TEST_ASSERT(splitter.getNumBytesReturned() == 0);
    TEST_ASSERT(splitter.getNumBytesProcessed() == 0);
    bool success = splitter.getNext(substring);
    TEST_ASSERT(success);
    TEST_ASSERT(substring == "");
    success = splitter.getNext(substring);
    TEST_ASSERT(!success);

    TEST_ASSERT(splitter.getNumSubstringsReturned() == 1);
    TEST_ASSERT(splitter.getNumBytesReturned() == 0);
    TEST_ASSERT(splitter.getNumBytesProcessed() == 0);
}

TEST_CASE(testStreamSplitterInputValidation)
{
    // delimiter must be nonempty string
    TEST_EXCEPTION(streamSplitterTestRunner(10, 10, "", 2));

    // buffer too small to support delimiter
    TEST_EXCEPTION(streamSplitterTestRunner(10, 10, " ", 2));
    TEST_ASSERT(streamSplitterTestRunner(10, 10, " ", 3));

    TEST_EXCEPTION(streamSplitterTestRunner(10, 10, "  ", 4));
    TEST_ASSERT(streamSplitterTestRunner(10, 10, "  ", 5));

    TEST_EXCEPTION(streamSplitterTestRunner(10, 10, "abc", 6));
    TEST_ASSERT(streamSplitterTestRunner(10, 10, "abc", 7));
}

int main(int, char**)
{
    TEST_CHECK(testStreamSplitterEmpty);
    TEST_CHECK(testStreamSplitter);
    TEST_CHECK(testStreamSplitterInputValidation);
}
