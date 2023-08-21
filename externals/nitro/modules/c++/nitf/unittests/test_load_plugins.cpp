/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <vector>
#include <iostream>
#include <string>

#include <import/nitf.h>
#include <import/nitf.hpp>
#include <nitf/UnitTests.hpp>

#include "TestCase.h"

static void retrieveTREHandler(const std::string& testName, const char* tre)
{
    nitf_Error error;

    auto const reg = nitf::PluginRegistry::getInstance(error);
    TEST_ASSERT(reg != nullptr);

    //nitf::HashTable::print(*(reg->treHandlers));

    int bad = 0;
    auto const test_main_ = nitf::PluginRegistry::retrieveTREHandler(*reg, tre, bad, error);
    TEST_ASSERT_EQ(0, bad);
    TEST_ASSERT(test_main_ != nullptr);
}

static const auto& all_TREs()
{
    static const std::vector<std::string> retval
    {
    #if _MSC_VER && NITRO_PCH
        // only build a handful in Visual Studio
        "ACCHZB", "ACCPOB", "ACFTA", "AIMIDB", "CSCRNA", "CSEXRB", "ENGRDA", "HISTOA", "JITCID", "PTPRAA", "RPFHDR",
    #else
        "ACCHZB", "BANDSB", "CSDIDA", "GEOLOB", "JITCID", "NBLOCA", "PIAPEB", "REGPTB", "RSMIDA", "STEROB",
        "ACCPOB", "BCKGDA", "CSEPHA", "GEOPSB", "MAPLOB", "OBJCTA", "PIAPRC", "RPC00B", "RSMPCA", "STREOB",
        "ACCVTB", "BLOCKA", "CSEXRA", "GRDPSB", "MATESA", "OFFSET", "PIAPRD", "RPFDES", "RSMPIA", "TEST_DES",
        "ACFTA", "BNDPLB", "CSPROA", "HISTOA", "MENSRA", "PATCHA", "PIATGA", "RPFHDR", "SECTGA", "TRGTA",
        "ACFTB", "CCINFA", "CSSFAA", "ICHIPB", "MENSRB", "PATCHB", "PIATGB", "RPFIMG", "SENSRA", "USE00A",
        "AIMIDA", "CLCTNA", "CSSHPA", "IMASDA", "MPDSRA", "PIAEQA", "PIXQLA", "RSMAPA", "SENSRB",
        "AIMIDB", "CLCTNB", "ENGRDA", "IMGDTA", "MSDIRA", "PIAEVA", "PLTFMA", "RSMDCA", "SNSPSB",
        "AIPBCA", "CMETAA", "EXOPTA", "IMRFCA", "MSTGTA", "PIAIMB", "PRADAA", "RSMECA", "SNSRA",
        "ASTORA", "CSCCGA", "EXPLTA", "IOMAPA", "MTIRPA", "PIAIMC", "PRJPSB", "RSMGGA", "SOURCB",
        "BANDSA", "CSCRNA", "EXPLTB", "J2KLRA", "MTIRPB", "PIAPEA", "PTPRAA", "RSMGIA", "STDIDC",
    #endif
    };
    return retval;
}

TEST_CASE(test_retrieveTREHandler)
{
    for (const auto& tre : all_TREs())
    {
        retrieveTREHandler(testName, tre.c_str());
    }
}

TEST_CASE(test_load_PTPRAA)
{
    retrieveTREHandler(testName, "PTPRAA");
}
TEST_CASE(test_load_ENGRDA)
{
    retrieveTREHandler(testName, "ENGRDA");
}

TEST_CASE(test_load_all_TREs)
{
    const nitf::TRE tre("ACCPOB");

    for (const auto& tre : all_TREs())
    {
        // TREs are quite the same thing as an arbitrary "plug in;" the underlying
        // infrastructure is all built on shared-libraries/DLLs, but details differ.
        //
        // As a result, we can't expect loadPlugin() will "just work" on a TRE name.
        // Unfortunately, the behavior is different on Windows and Linux. :-(
        #if _WIN32
        // Keep this around for now as it works ... but it's not necessarily correct.
        // Mostly an excuse to exercise more code.
        try
        {
            nitf::PluginRegistry::loadPlugin(tre);
        }
        catch (const nitf::NITFException& ex)
        {
            TEST_FAIL_MSG(ex.toString());
        }
        #endif // _WIN32

        TEST_ASSERT(nitf::PluginRegistry::treHandlerExists(tre));
    }
}

TEST_MAIN(
    nitf::Test::setNitfPluginPath();

    TEST_CHECK(test_load_PTPRAA);
    TEST_CHECK(test_load_ENGRDA);    
    TEST_CHECK(test_retrieveTREHandler);
    TEST_CHECK(test_load_all_TREs);
)