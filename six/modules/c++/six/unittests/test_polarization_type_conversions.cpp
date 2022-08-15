/* =========================================================================
* This file is part of six.sidd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <vector>

#include <import/str.h>

#include "TestCase.h"
#include <six/Enums.h>
#include <six/Utilities.h>

template<typename TSixEnum>
static void test_EnumConstructor(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;
    const auto pType = six::Enum::toType<TSixEnum>(strType);
    TEST_ASSERT(six::Enum::toString(pType) == strType);
    TEST_ASSERT_EQ(pType, type);
}
TEST_CASE(EnumConstructor)
{
    test_EnumConstructor<six::DualPolarizationType>(testName, "OTHER", six::DualPolarizationType::OTHER);
    test_EnumConstructor<six::DualPolarizationType>(testName, "UNKNOWN", six::DualPolarizationType::UNKNOWN);
    test_EnumConstructor<six::DualPolarizationType>(testName, "V_V", six::DualPolarizationType::V_V);
    test_EnumConstructor<six::DualPolarizationType>(testName, "E_V", six::DualPolarizationType::E_V); // SICD 1.3

    test_EnumConstructor<six::PolarizationType>(testName, "OTHER", six::PolarizationType::OTHER);
    test_EnumConstructor<six::PolarizationType>(testName, "UNKNOWN", six::PolarizationType::UNKNOWN);
    test_EnumConstructor<six::PolarizationType>(testName, "X", six::PolarizationType::X); // SICD 1.3

    test_EnumConstructor<six::PolarizationSequenceType>(testName, "OTHER", six::PolarizationSequenceType::OTHER);
    test_EnumConstructor<six::PolarizationSequenceType>(testName, "UNKNOWN", six::PolarizationSequenceType::UNKNOWN);
    test_EnumConstructor<six::PolarizationSequenceType>(testName, "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_EnumConstructor<six::PolarizationSequenceType>(testName, "X", six::PolarizationSequenceType::X); // SICD 1.3
}

template<typename TSixEnum>
static void test_toType_(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;
    const auto fromToType = six::Enum::toType<TSixEnum>(strType);
    TEST_ASSERT_EQ(fromToType, type);
    auto str = six::Enum::toString(fromToType);
    TEST_ASSERT_EQ(str, strType);

    const auto fromCtor = six::Enum::toType<TSixEnum>(strType);
    TEST_ASSERT_EQ(fromToType, fromCtor);
    str = six::Enum::toString(fromCtor);
    TEST_ASSERT_EQ(str, strType);
}
template<typename TSixEnum>
static void test_toType(const std::string& testName, size_t sz)
{
    test_toType_<TSixEnum>(testName, "UNKNOWN", TSixEnum::UNKNOWN);
    test_toType_<TSixEnum>(testName, "OTHER", TSixEnum::OTHER);

    auto&& map = TSixEnum::string_to_value_();
    TEST_ASSERT_EQ(map.size(), sz);
    for (auto&& kv : map)
    {
        const TSixEnum fromInt(kv.second);
        const auto toType = six::Enum::toType<TSixEnum>(kv.first);
        TEST_ASSERT_EQ(toType, fromInt);

        if (fromInt != TSixEnum::NOT_SET)
        {
            test_toType_<TSixEnum >(testName, kv.first, fromInt);
        }
    }
}
TEST_CASE(ToType)
{
    test_toType_<six::DualPolarizationType>(testName, "V_V", six::DualPolarizationType::V_V);
    test_toType_<six::DualPolarizationType>(testName, "E_V", six::DualPolarizationType::E_V); // SICD 1.3
    test_toType<six::DualPolarizationType>(testName, 85);

    test_toType_<six::PolarizationType>(testName, "X", six::PolarizationType::X);  // SICD 1.3
    test_toType<six::PolarizationType>(testName, 12);

    test_toType_<six::PolarizationSequenceType>(testName, "X", six::PolarizationSequenceType::X);  // SICD 1.3
    test_toType_<six::PolarizationSequenceType>(testName, "SEQUENCE", six::PolarizationSequenceType::SEQUENCE); 
    test_toType<six::PolarizationSequenceType>(testName, 13);
}

template<typename TSixEnum>
static void test_toType_OTHER(const std::string& testName)
{
    (void)testName;

    const TSixEnum not_set;
    TEST_ASSERT_EQ(not_set, TSixEnum::NOT_SET);

    auto fromToType = six::Enum::toType<TSixEnum>("OTHER");
    TEST_ASSERT_EQ(fromToType, TSixEnum::OTHER);

    TEST_EXCEPTION(six::Enum::toType<TSixEnum>("OTHER:abc"));
}
TEST_CASE(ToType_OTHER)
{
    test_toType_OTHER<six::PolarizationType>(testName);
    test_toType_OTHER<six::PolarizationSequenceType>(testName);
}
TEST_CASE(DualPolarizationType_ToType_OTHER)
{
    test_toType_OTHER<six::DualPolarizationType>("DualPolarizationType_ToType_OTHER");

    auto toTypeDual = six::Enum::toType<six::DualPolarizationType>("V_OTHER");
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::V_OTHER);
    toTypeDual = six::Enum::toType<six::DualPolarizationType>("OTHER_V");
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::OTHER_V);
    toTypeDual = six::Enum::toType<six::DualPolarizationType>("OTHER_OTHER");
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::OTHER_OTHER);

    TEST_EXCEPTION(six::Enum::toType<six::DualPolarizationType>("OTHER_xyz")); // no "top level" OTHER.*
}

template<typename TSixEnum>
static void test_six_toType_(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;
    const auto fromToType = six::toType<TSixEnum>(strType);
    TEST_ASSERT_EQ(fromToType, type);
    const auto str = six::toString<TSixEnum>(fromToType);
    TEST_ASSERT_EQ(str, strType);
    TEST_ASSERT_EQ(str, six::toString(fromToType)); // no template parameter
}
template<typename TSixEnum>
static void test_six_toType(const std::string& testName, size_t sz)
{
    test_six_toType_<TSixEnum>(testName, "OTHER", TSixEnum::OTHER);

    auto&& map = TSixEnum::string_to_value_();
    TEST_ASSERT_EQ(map.size(), sz);
    for (auto&& kv : map)
    {
        const TSixEnum fromInt(kv.second);
        if (fromInt != TSixEnum::NOT_SET)
        {
            auto s = kv.first;
            str::replace(s, "_", ":");
            const auto toType = six::toType<TSixEnum>(s);
            TEST_ASSERT_EQ(toType, fromInt);

            test_six_toType_<TSixEnum>(testName, s, fromInt);
        }
    }
}
TEST_CASE(SixToType)
{
    test_six_toType_<six::DualPolarizationType>(testName, "V:V", six::DualPolarizationType::V_V);
    test_six_toType_<six::DualPolarizationType>(testName, "E:V", six::DualPolarizationType::E_V); // SICD 1.3
    test_six_toType_<six::DualPolarizationType>(testName, "UNKNOWN", six::DualPolarizationType::UNKNOWN);
    test_six_toType<six::DualPolarizationType>(testName, 85);

    test_six_toType_<six::PolarizationType>(testName, "X", six::PolarizationType::X);  // SICD 1.3
    test_six_toType_<six::PolarizationType>(testName, "OTHER_abc", six::PolarizationType::OTHER); // SIDD 3.0/SICD 1.3
    test_six_toType<six::PolarizationType>(testName, 12);

    test_six_toType_<six::PolarizationSequenceType>(testName, "X", six::PolarizationSequenceType::X);  // SICD 1.3
    test_six_toType_<six::PolarizationSequenceType>(testName, "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_six_toType_<six::PolarizationSequenceType>(testName, "UNKNOWN", six::PolarizationSequenceType::UNKNOWN);
    test_six_toType_<six::PolarizationSequenceType>(testName, "OTHER_abc", six::PolarizationSequenceType::OTHER); // SIDD 3.0/SICD 1.3
    test_six_toType<six::PolarizationSequenceType>(testName, 13);

    auto toTypeDual = six::Enum::toType<six::DualPolarizationType>("V_OTHER");
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::V_OTHER);
    toTypeDual = six::toType<six::DualPolarizationType>("V:OTHER_xyz"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::V_OTHER);
    toTypeDual = six::Enum::toType<six::DualPolarizationType>("OTHER_V");
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::OTHER_V);
    toTypeDual = six::toType<six::DualPolarizationType>("OTHER_xyz:V"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::OTHER_V);
    toTypeDual = six::toType<six::DualPolarizationType>("OTHER_abc:OTHER"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::OTHER_OTHER);
    toTypeDual = six::toType<six::DualPolarizationType>("OTHER:OTHER_xyz"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::OTHER_OTHER);
    toTypeDual = six::toType<six::DualPolarizationType>("OTHER_abc:OTHER_xyz"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toTypeDual, six::DualPolarizationType::OTHER_OTHER);
}

template<typename TSixEnum>
static void test_ToString_(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;
    {
        const auto polarizationString = six::Enum::toString(type);
        TEST_ASSERT_EQ(strType, polarizationString);
        auto pType = six::Enum::toType<TSixEnum>(polarizationString);
        TEST_ASSERT_EQ(pType, type);
    }
    {
        const auto polarizationString = six::Enum::toString(type);
        TEST_ASSERT_EQ(strType, polarizationString);
        auto pType = six::Enum::toType<TSixEnum>(polarizationString);
        TEST_ASSERT_EQ(pType, type);
    }
}
template<typename TSixEnum>
static void test_ToString(const std::string& testName)
{
    test_ToString_<TSixEnum>(testName, "UNKNOWN", TSixEnum::UNKNOWN);
    test_ToString_<TSixEnum>(testName, "OTHER", TSixEnum::OTHER);
}
TEST_CASE(ToString)
{
    test_ToString_<six::DualPolarizationType>(testName, "V_V", six::DualPolarizationType::V_V);
    test_ToString_<six::DualPolarizationType>(testName, "E_V", six::DualPolarizationType::E_V); // SICD 1.3
    test_ToString<six::DualPolarizationType>("ToString");

    test_ToString_<six::PolarizationType>(testName, "V", six::PolarizationType::V);
    test_ToString_<six::PolarizationType>(testName, "X", six::PolarizationType::X); // SICD 1.3
    test_ToString<six::PolarizationType>("ToString");

    test_ToString_<six::PolarizationSequenceType>(testName, "V", six::PolarizationSequenceType::V);
    test_ToString_<six::PolarizationSequenceType>(testName, "X", six::PolarizationSequenceType::X); // SICD 1.3
    test_ToString_<six::PolarizationSequenceType>(testName, "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_ToString<six::PolarizationSequenceType>("ToString");
}

TEST_CASE(DualPolarizationType_ToString_OTHER)
{
    const six::DualPolarizationType not_set;
    TEST_ASSERT_EQ(not_set, six::DualPolarizationType::NOT_SET);
    TEST_ASSERT_EQ("NOT_SET", six::Enum::toString(not_set, false /*throw_if_not_set*/));
    TEST_EXCEPTION(six::Enum::toString(not_set, true /*throw_if_not_set*/));
    TEST_ASSERT(six::Enum::toString(not_set) == "NOT_SET");

    auto toType_DualPolarization = six::Enum::toType<six::DualPolarizationType>("OTHER");
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER);
    TEST_ASSERT_EQ("OTHER", six::Enum::toString(toType_DualPolarization));

    toType_DualPolarization = six::Enum::toType<six::DualPolarizationType>("V_OTHER");
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::V_OTHER);
    TEST_ASSERT_EQ("V_OTHER", six::Enum::toString(toType_DualPolarization));

    toType_DualPolarization = six::Enum::toType<six::DualPolarizationType>("OTHER_V");
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER_V);
    TEST_ASSERT_EQ("OTHER_V", six::Enum::toString(toType_DualPolarization));

    toType_DualPolarization = six::Enum::toType<six::DualPolarizationType>("OTHER_OTHER");
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER_OTHER);
    TEST_ASSERT_EQ("OTHER_OTHER", six::Enum::toString(toType_DualPolarization));
}
TEST_CASE(DualPolarizationType_SixToType_OTHER)
{
    auto toType_DualPolarization = six::toType<six::DualPolarizationType>("OTHER");
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER);
    TEST_ASSERT_EQ("OTHER", six::Enum::toString(toType_DualPolarization));

    TEST_EXCEPTION(six::toType<six::DualPolarizationType>("OTHER_abc")); // no "top level" OTHER.*

    toType_DualPolarization = six::toType<six::DualPolarizationType>("OTHER:V");
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER_V);
    TEST_ASSERT_EQ("OTHER_V", six::Enum::toString(toType_DualPolarization));
    TEST_ASSERT_EQ("OTHER:V", six::toString(toType_DualPolarization));

    toType_DualPolarization = six::toType<six::DualPolarizationType>("V:OTHER_xyz"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::V_OTHER);
    TEST_ASSERT_EQ("V_OTHER", six::Enum::toString(toType_DualPolarization));
    TEST_ASSERT_EQ("V:OTHER_xyz", six::toString(toType_DualPolarization));

    toType_DualPolarization = six::toType<six::DualPolarizationType>("OTHER_xyz:V"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER_V);
    TEST_ASSERT_EQ("OTHER_V", six::Enum::toString(toType_DualPolarization));
    TEST_ASSERT_EQ("OTHER_xyz:V", six::toString(toType_DualPolarization));

    toType_DualPolarization = six::toType<six::DualPolarizationType>("OTHER:OTHER");
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER_OTHER);
    TEST_ASSERT_EQ("OTHER_OTHER", six::Enum::toString(toType_DualPolarization));
    TEST_ASSERT_EQ("OTHER:OTHER", six::toString(toType_DualPolarization));

    toType_DualPolarization = six::toType<six::DualPolarizationType>("OTHER_abc:OTHER_xyz"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(toType_DualPolarization, six::DualPolarizationType::OTHER_OTHER);
    TEST_ASSERT_EQ("OTHER_OTHER", six::Enum::toString(toType_DualPolarization));
    TEST_ASSERT_EQ("OTHER_abc:OTHER_xyz", six::toString(toType_DualPolarization));

    toType_DualPolarization.other_ = "abc";
    TEST_EXCEPTION(six::toString(toType_DualPolarization));

    toType_DualPolarization.other_ = "OTHER_xyz:V";
    TEST_EXCEPTION(six::toString(toType_DualPolarization)); // OTHER_OTHER, not OTHER_V
}

template<typename TSixEnum>
static void test_six_toString_(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;

    const std::string polarizationString = six::toString<TSixEnum>(type);
    if (type != TSixEnum::OTHER)
    {
        TEST_ASSERT_EQ(strType, polarizationString);
    }
    TEST_ASSERT_EQ(polarizationString, six::toString(type)); // no template parameter

    auto pType = six::toType<TSixEnum>(polarizationString);
    TEST_ASSERT_EQ(pType, type);

    pType.other_ = "abc";
    TEST_EXCEPTION(six::toString(pType));
}
template<typename TSixEnum>
static void test_six_toString(const std::string& testName)
{
    test_six_toString_<TSixEnum>(testName, "UNKNOWN", TSixEnum::UNKNOWN);
    test_six_toString_<TSixEnum>(testName, "OTHER", TSixEnum::OTHER);
}
TEST_CASE(SixToString)
{
    test_six_toString_<six::DualPolarizationType>(testName, "V:V", six::DualPolarizationType::V_V);
    test_six_toString_<six::DualPolarizationType>(testName, "E:V", six::DualPolarizationType::E_V); // SICD 1.3
    test_six_toString<six::DualPolarizationType>(testName);

    test_six_toString_<six::PolarizationType>(testName, "V", six::PolarizationType::V);
    test_six_toString_<six::PolarizationType>(testName, "X", six::PolarizationType::X); // SICD 1.3
    test_six_toString_<six::PolarizationType>(testName, "OTHER_abc", six::PolarizationType::OTHER); // SICD 1.3

    test_six_toString_<six::PolarizationSequenceType>(testName, "V", six::PolarizationSequenceType::V);
    test_six_toString_<six::PolarizationSequenceType>(testName, "X", six::PolarizationSequenceType::X); // SICD 1.3
    test_six_toString_<six::PolarizationSequenceType>(testName, "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_six_toString_<six::PolarizationSequenceType>(testName, "OTHER_abc", six::PolarizationSequenceType::OTHER); // SICD 1.3
    test_six_toString<six::PolarizationSequenceType>(testName);
}

template<typename TSixEnum>
static void test_NotSet(const std::string& testName)
{
    (void)testName;

    TSixEnum pType;
    TEST_ASSERT_EQ(pType, TSixEnum::NOT_SET);
    auto polarizationString = six::Enum::toString(pType);
    TEST_ASSERT_EQ(polarizationString, "NOT_SET");
    TEST_EXCEPTION(polarizationString = six::Enum::toString(pType, true /*throw_if_not_set*/));
    polarizationString = six::Enum::toString(pType, false /*throw_if_not_set*/);
    TEST_ASSERT_EQ(polarizationString, "NOT_SET");

    pType = six::Enum::toType<TSixEnum>("NOT SET");
    TEST_ASSERT_EQ(pType, TSixEnum::NOT_SET);
    pType = six::Enum::toType<TSixEnum>("NOT_SET");
    TEST_ASSERT_EQ(pType, TSixEnum::NOT_SET);

    TEST_EXCEPTION(six::toType<TSixEnum>("NOT_SET"));

    TEST_EXCEPTION(six::toString(pType));
}
TEST_CASE(NotSet)
{
    test_NotSet<six::DualPolarizationType>("NotSet");
    test_NotSet<six::PolarizationType>("NotSet");
    test_NotSet<six::PolarizationSequenceType>("NotSet");
}

template<typename TSixEnum>
static void test_EqInt_(const std::string& testName, const std::string& strType, TSixEnum type, int enumValue)
{
    (void)testName;

    const auto fromStrCtor = six::Enum::toType<TSixEnum>(strType);
    TEST_ASSERT_EQ(enumValue, fromStrCtor);
    const int value = fromStrCtor;
    TEST_ASSERT_EQ(enumValue, value);

    const decltype(type)fromIntCtor(six::Enum::cast<TSixEnum>(value));
    TEST_ASSERT_EQ(enumValue, fromIntCtor);
    TEST_ASSERT(six::Enum::toString(fromIntCtor) == strType);
    TEST_ASSERT_EQ(fromIntCtor, type);

    TEST_ASSERT_EQ(fromStrCtor, fromIntCtor);
};
template<typename TSixEnum>
static void test_EqInt(const std::string& testName, int unknownEnumValue)
{
    test_EqInt_<TSixEnum>(testName, "UNKNOWN", TSixEnum::UNKNOWN, unknownEnumValue);
    test_EqInt_<TSixEnum>(testName, "OTHER", TSixEnum::OTHER, 1);
    test_EqInt_<TSixEnum>(testName, "NOT_SET", TSixEnum::NOT_SET, six::NOT_SET_VALUE);

    // These will throw, at least for the polarization types
    static const std::vector<int> invalidIntValuesToTest{ -1, 0, six::NOT_SET_VALUE - 1 };
    for (auto&& v : invalidIntValuesToTest)
    {
        TEST_EXCEPTION(six::Enum::cast<TSixEnum>(v));
    }
}
TEST_CASE(EqInt)
{
    test_EqInt_<six::PolarizationType>(testName, "V", six::PolarizationType::V, 2);
    test_EqInt_<six::PolarizationType>(testName, "X", six::PolarizationType::X, 7); // SICD 1.3
    test_EqInt<six::PolarizationType>(testName, 6 /*unknownEnumValue*/);

    test_EqInt_<six::PolarizationSequenceType>(testName, "V", six::PolarizationSequenceType::V, 2);
    test_EqInt_<six::PolarizationSequenceType>(testName, "X", six::PolarizationSequenceType::X, 8); // SICD 1.3
    test_EqInt_<six::PolarizationSequenceType>(testName, "SEQUENCE", six::PolarizationSequenceType::SEQUENCE, 7);
    test_EqInt<six::PolarizationSequenceType>(testName, 6 /*unknownEnumValue*/);
}
TEST_CASE(DualPolarizationType_EqInt)
{
    test_EqInt_<six::DualPolarizationType>(testName, "V_V", six::DualPolarizationType::V_V, 2);
    test_EqInt_<six::DualPolarizationType>(testName, "E_V", six::DualPolarizationType::E_V, 56); // SICD 1.3
    test_EqInt_<six::DualPolarizationType>(testName, "OTHER_V", six::DualPolarizationType::OTHER_V, 75);
    test_EqInt_<six::DualPolarizationType>(testName, "OTHER_OTHER", six::DualPolarizationType::OTHER_OTHER, 83);
    test_EqInt<six::DualPolarizationType>(testName, 18 /*unknownEnumValue*/);
}

TEST_CASE(DualPolarization)
{
    // https://pcf-om-mil-bb5cb050-f7c0-44fc-b114-b886abb80450.s3.us-east-1.amazonaws.com/doc/Document/NGA.STND.0024-1_1.3.0.pdf?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAWDCVGY5THCA2IKOI%2F20220531%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20220531T154720Z&X-Amz-Expires=900&X-Amz-SignedHeaders=host&response-cache-control=900&response-content-disposition=NGA.STND.0024-1_1.3.0.pdf&X-Amz-Signature=dc2de9c048244ce338da927ccaa26567ca8c5afc9c36aa384c21dc2e8faeec04
    // Allowed values include the form TX:RCV that is formed from one  TX value and one RCV value.
    // Allowed TX values: “V”, “H”, “X”, “Y”, “S”, “E”, “RHC”, “LHC”, “OTHER*”
    // Allowed RCV values:  “V”, “H”, “X”, “Y”, “S”, “E”, “RHC”, “LHC”, “OTHER*”,    

    auto&& map = six::PolarizationType::string_to_value_();
    for (auto&& tx : map)
    {
        const auto txType = six::Enum::toType<six::PolarizationType>(tx.first);
        TEST_ASSERT_EQ(tx.second, txType);
        if ((txType == six::PolarizationType::NOT_SET) || (txType == six::PolarizationType::UNKNOWN))
        {
            continue;
        }
        for (auto&& rcv : map)
        {
            const auto rcvType = six::Enum::toType<six::PolarizationType>(rcv.first);
            TEST_ASSERT_EQ(rcv.second, rcvType);
            if ((rcvType == six::PolarizationType::NOT_SET) || (rcvType == six::PolarizationType::UNKNOWN))
            {
                continue;
            }
            auto strType = six::Enum::toString(txType) + "_" + six::Enum::toString(rcvType);

            auto fromToType = six::Enum::toType<six::DualPolarizationType>(strType);
            auto str = six::Enum::toString(fromToType);
            TEST_ASSERT_EQ(str, strType);
            test_toType_(testName, strType, fromToType);

            str::replace(strType, "_", ":");
            fromToType = six::toType<six::DualPolarizationType>(strType);
            str = six::toString(fromToType);
            TEST_ASSERT_EQ(str, strType);
            test_six_toType_(testName, strType, fromToType);
        }
    }
}

TEST_MAIN(
    TEST_CHECK(DualPolarization);
    TEST_CHECK(DualPolarizationType_EqInt);
    TEST_CHECK(DualPolarizationType_SixToType_OTHER);
    TEST_CHECK(DualPolarizationType_ToString_OTHER);
    TEST_CHECK(DualPolarizationType_ToType_OTHER);
    TEST_CHECK(EnumConstructor);
    TEST_CHECK(EqInt);
    TEST_CHECK(NotSet);
    TEST_CHECK(SixToString);
    TEST_CHECK(SixToType);
    TEST_CHECK(ToString);
    TEST_CHECK(ToType);
    TEST_CHECK(ToType_OTHER);
    )
