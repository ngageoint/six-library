/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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

#include <import/sys.h>
#include <import/io.h>
#include <import/zip.h>
#include <io/ReadUtils.h>

#include <TestCase.h>

static std::filesystem::path find_unittest_file(const std::filesystem::path& name)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "zip" / "unittests";
    return sys::test::findGITModuleFile("coda-oss", unittests, name);
}

static std::string txt_to_gz(const std::filesystem::path& txt_path)
{
    const auto pid = sys::OS().getSpecialEnv("PID");
    return txt_path.stem().string() + pid + txt_path.extension().string() + ".gz";
}

static std::string gz_to_txt(const std::filesystem::path& gz_path)
{
    const auto pid = sys::OS().getSpecialEnv("PID");
    return gz_path.stem().string() + pid + gz_path.extension().string() + ".txt";
}

TEST_CASE(gzip)
{
    static const auto inputPath = find_unittest_file("text.txt");

    const std::filesystem::path outputName_(txt_to_gz(inputPath));
    const auto outputName = "TEST_" + outputName_.stem().string() + "_TMP" + outputName_.extension().string(); // see .gitignore
    const auto outputDir = inputPath.parent_path();
    const auto origOutputPath = outputDir / outputName;
    auto outputPath = origOutputPath;

    {
        io::FileInputStream input(inputPath.string());
        zip::GZipOutputStream output(outputPath.string());

        input.streamTo(output);
        input.close();
        output.close();
    }
    
    std::vector<coda_oss::byte> buffer;
    io::readFileContents(outputPath, buffer);
    TEST_ASSERT_EQ(32, buffer.size());
    {
        zip::GZipInputStream input(outputPath.string());
        outputPath = outputDir / gz_to_txt(outputPath);
        io::FileOutputStream output(outputPath.string());
        while (input.streamTo(output, 8192)) ;

        input.close();
        output.close();

        const auto str = io::readFileContents(outputPath.string());
        TEST_ASSERT_EQ("Hello World!", str);
    }

    remove(origOutputPath);
    remove(outputPath);
}

TEST_CASE(gunzip)
{
    static const auto inputPath = find_unittest_file("test.gz");

    const std::filesystem::path outputName_(gz_to_txt(inputPath));
    const auto outputName = "TEST_" + outputName_.stem().string() + "_TMP" + outputName_.extension().string(); // see .gitignore
    const auto outputPath = inputPath.parent_path() / outputName;
                
    zip::GZipInputStream input(inputPath.string());
    io::FileOutputStream output(outputPath.string());
    while ( input.streamTo(output, 8192) );

    input.close();
    output.close();

    const auto str = io::readFileContents(outputPath.string());
    TEST_ASSERT_EQ("Hello World!", str);
    
    remove(outputPath);
}

TEST_MAIN(
    TEST_CHECK(gzip);
    TEST_CHECK(gunzip);
)