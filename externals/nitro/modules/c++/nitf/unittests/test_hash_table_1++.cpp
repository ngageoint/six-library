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
#include <map>
#include <string>

#include <import/nitf.hpp>

#include "TestCase.h"

/*  For this first test case, the DATA is a simple char*   */
/*  This case should be trivial since a data pointer is    */
/*  always the same size in C                              */

TEST_CASE(test_hash_table_1)
{
    /*  A key buffer  */
    std::string keyBuf;

    nitf::HashTable hashTable(4);

    /*  We didnt allocate the valueBuf -- its static          */
    /*  As a result, we need to notify our hash table NOT to  */
    /*  destroy it on failure                                 */
    hashTable.setPolicy(NITF_DATA_RETAIN_OWNER);

    const std::map<std::string, std::string> config{
        { "", ""}, { "k1", "v1"}, { "k2", "v2"}, { "k3", "v3"}, { "k4", "v4"},
    };

    std::vector<std::string> values;
    values.reserve(config.size()); // don't move values

    /*  Scan the configuration file, and read into the buffers  */
    for (const auto& c : config)
    {
        keyBuf = c.first;
        values.push_back(c.second);
        auto data = static_cast<const NITF_DATA*>(values.back().c_str());

        try
        {
            /*  Now comes the important part -- insert  */
            hashTable.insert(keyBuf, const_cast<NITF_DATA*>(data));
        }
        catch (const except::NoSuchKeyException&)
        {
            TEST_ASSERT(keyBuf.empty());
        }
    }

    /*  Now, lets be expansive and print the list  */
    hashTable.print();

    /*  Open the search file -- this contains keys to search for  */
    const std::vector<std::string> search{ "", "k1", "k2", "k3", "k4", "NOT FOUND" };
    for (auto& s : search)
    {
        keyBuf = s;
        if (!keyBuf.empty())
        {
            try
            {
                /*  Find the key/value pair  */
                const nitf::Pair where = hashTable.find(keyBuf);

                const std::string key = where.getKey();
                TEST_ASSERT_EQ(keyBuf, key);

                /*  Its there in the hash!!  */
                const std::string data = static_cast<const char*>(where.getData());
                TEST_ASSERT_EQ(static_cast<size_t>(2), data.length());
                TEST_ASSERT_EQ('v', data[0]);
                TEST_ASSERT_EQ(static_cast<size_t>(2), keyBuf.length());
                TEST_ASSERT_EQ(keyBuf[1], data[1]);
            }
            catch (const except::NoSuchKeyException& t)
            {
                TEST_ASSERT_EQ_STR("NOT FOUND", keyBuf);
                const auto message = t.getMessage();
                TEST_ASSERT_EQ(message, keyBuf);
            }
        }
    }
}


TEST_CASE(test_hash_table_iterator)
{
    /*  A key buffer  */
    std::string keyBuf;

    nitf::HashTable hashTable(4);

    /*  We didnt allocate the valueBuf -- its static          */
    /*  As a result, we need to notify our hash table NOT to  */
    /*  destroy it on failure                                 */
    hashTable.setPolicy(NITF_DATA_RETAIN_OWNER);

    const std::map<std::string, std::string> config{
        { "k1", "v1"}, { "k2", "v2"}, { "k3", "v3"}, { "k4", "v4"},
    };

    std::vector<std::string> values;
    values.reserve(config.size()); // don't move values

    /*  Scan the configuration file, and read into the buffers  */
    for (const auto& c : config)
    {
        keyBuf = c.first;
        values.push_back(c.second);
        auto data = static_cast<const NITF_DATA*>(values.back().c_str());

        /*  Now comes the important part -- insert  */
        hashTable.insert(keyBuf, const_cast<NITF_DATA*>(data));
    }

    for (const auto& where : hashTable)
    {
        const std::string key = where.getKey();
        TEST_ASSERT_EQ(static_cast<size_t>(2), key.length());
        TEST_ASSERT_EQ('k', key[0]);
        TEST_ASSERT(isdigit(key[1]) != 0);

        const std::string data = static_cast<const char*>(where.getData());
        TEST_ASSERT_EQ(static_cast<size_t>(2), data.length());
        TEST_ASSERT_EQ('v', data[0]);
        TEST_ASSERT(isdigit(data[1]) != 0);
        TEST_ASSERT_EQ(key[1], data[1]);
    }
}

TEST_MAIN(
    (void)argc;
    (void)argv;
    TEST_CHECK(test_hash_table_1);
    TEST_CHECK(test_hash_table_iterator);
)