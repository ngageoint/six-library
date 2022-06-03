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
    const decltype(type) pType(strType);
    TEST_ASSERT(pType == strType);
    TEST_ASSERT_EQ(pType, type);
}
TEST_CASE(EnumConstructor)
{
    test_EnumConstructor<six::DualPolarizationType>("EnumConstructor", "UNKNOWN", six::DualPolarizationType::UNKNOWN);
    test_EnumConstructor<six::DualPolarizationType>("EnumConstructor", "V_V", six::DualPolarizationType::V_V);
    test_EnumConstructor<six::DualPolarizationType>("EnumConstructor", "E_V", six::DualPolarizationType::E_V); // SICD 1.3

    test_EnumConstructor<six::PolarizationType>("EnumConstructor", "OTHER", six::PolarizationType::OTHER);
    test_EnumConstructor<six::PolarizationType>("EnumConstructor", "UNKNOWN", six::PolarizationType::UNKNOWN);
    test_EnumConstructor<six::PolarizationType>("EnumConstructor", "X", six::PolarizationType::X); // SICD 1.3

    test_EnumConstructor<six::PolarizationSequenceType>("EnumConstructor", "OTHER", six::PolarizationSequenceType::OTHER);
    test_EnumConstructor<six::PolarizationSequenceType>("EnumConstructor", "UNKNOWN", six::PolarizationSequenceType::UNKNOWN);
    test_EnumConstructor<six::PolarizationSequenceType>("EnumConstructor", "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_EnumConstructor<six::PolarizationSequenceType>("EnumConstructor", "X", six::PolarizationSequenceType::X); // SICD 1.3
}

template<typename TSixEnum>
static void test_toType_(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;
    const auto fromToType = TSixEnum::toType(strType);
    TEST_ASSERT_EQ(fromToType, type);
    auto str = fromToType.toString();
    TEST_ASSERT_EQ(str, strType);

    const decltype(type) fromCtor(strType);
    TEST_ASSERT_EQ(fromToType, fromCtor);
    str = fromCtor.toString();
    TEST_ASSERT_EQ(str, strType);
}
template<typename TSixEnum>
static void test_toType(const std::string& testName, size_t sz)
{
    test_toType_<TSixEnum>("ToType", "UNKNOWN", TSixEnum::UNKNOWN);
    test_toType_<TSixEnum>("ToType", "OTHER", TSixEnum::OTHER);

    const auto string_to_int = TSixEnum::string_to_int_();
    TEST_ASSERT_EQ(string_to_int.size(), sz);
    for (auto&& kv : string_to_int)
    {
        const TSixEnum fromInt(kv.second);
        const auto toType = TSixEnum::toType(kv.first);
        TEST_ASSERT_EQ(toType, fromInt);

        if (fromInt != TSixEnum::NOT_SET)
        {
            test_toType_<TSixEnum >(testName, kv.first, fromInt);
        }
    }
}
TEST_CASE(ToType)
{
    test_toType_<six::DualPolarizationType>("ToType", "V_V", six::DualPolarizationType::V_V);
    test_toType_<six::DualPolarizationType>("ToType", "E_V", six::DualPolarizationType::E_V); // SICD 1.3
    test_toType<six::DualPolarizationType>("ToType", 85);

    test_toType_<six::PolarizationType>("ToType", "X", six::PolarizationType::X);  // SICD 1.3
    test_toType<six::PolarizationType>("ToType", 12);

    test_toType_<six::PolarizationSequenceType>("ToType", "X", six::PolarizationSequenceType::X);  // SICD 1.3
    test_toType_<six::PolarizationSequenceType>("ToType", "SEQUENCE", six::PolarizationSequenceType::SEQUENCE); 
    test_toType<six::PolarizationSequenceType>("ToType", 13);
}

TEST_CASE(ToType_OTHER)
{
    auto fromToType = six::PolarizationType::toType("OTHER");
    TEST_ASSERT_EQ(fromToType, six::PolarizationType::OTHER);

    fromToType = six::PolarizationType::toType_imp_("OTHER"); // explicitly call _imp_()
    TEST_ASSERT_EQ(fromToType, six::PolarizationType::OTHER);

    // SICD 1.3 "OTHER.*"
    fromToType = six::PolarizationType::toType_imp_("OTHER_abc"); // explicitly call _imp_()
    TEST_ASSERT_EQ(fromToType, six::PolarizationType::OTHER);
    TEST_EXCEPTION(six::PolarizationType::toType_imp_("OTHER:abc")); // explicitly call _imp_()
    fromToType = six::PolarizationType::toType("OTHER_abc"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(fromToType, six::PolarizationType::OTHER);
    TEST_EXCEPTION(six::PolarizationType::toType("OTHER:abc"));

    six::PolarizationType OTHER_abc("OTHER_abc");
    TEST_ASSERT_EQ(OTHER_abc, six::PolarizationType::OTHER);
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
    test_six_toType_<TSixEnum>("SixToType", "OTHER", TSixEnum::OTHER);

    const auto string_to_int = TSixEnum::string_to_int_();
    TEST_ASSERT_EQ(string_to_int.size(), sz);
    for (auto&& kv : string_to_int)
    {
        const TSixEnum fromInt(kv.second);
        if ((fromInt != TSixEnum::NOT_SET) && (fromInt != TSixEnum::UNKNOWN))
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
    test_six_toType_<six::DualPolarizationType>("SixToType", "V:V", six::DualPolarizationType::V_V);
    test_six_toType_<six::DualPolarizationType>("SixToType", "E:V", six::DualPolarizationType::E_V); // SICD 1.3
    test_six_toType_<six::DualPolarizationType>("SixToType", "UNKNOWN", six::DualPolarizationType::UNKNOWN);
    test_six_toType<six::DualPolarizationType>("SixToType", 85);

    test_six_toType_<six::PolarizationType>("SixToType", "X", six::PolarizationType::X);  // SICD 1.3
    test_six_toType<six::PolarizationType>("SixToType", 12);
    TEST_EXCEPTION(six::toType<six::PolarizationType>("UNKNOWN")); // no conversion for six::PolarizationType::UNKNOWN

    test_six_toType_<six::PolarizationSequenceType>("SixToType", "X", six::PolarizationSequenceType::X);  // SICD 1.3
    test_six_toType_<six::PolarizationSequenceType>("SixToType", "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_six_toType_<six::PolarizationSequenceType>("SixToType", "UNKNOWN", six::PolarizationSequenceType::UNKNOWN);
    test_six_toType<six::PolarizationSequenceType>("SixToType", 13);
}

template<typename TSixEnum>
static void test_ToString_(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;
    {
        const std::string polarizationString = type;
        TEST_ASSERT_EQ(strType, polarizationString);
        decltype(type) pType(polarizationString);
        TEST_ASSERT_EQ(pType, type);
        pType = TSixEnum::toType(polarizationString);
        TEST_ASSERT_EQ(pType, type);
    }
    {
        const auto polarizationString = type.toString();
        TEST_ASSERT_EQ(strType, polarizationString);
        decltype(type) pType(polarizationString);
        TEST_ASSERT_EQ(pType, type);
        pType = TSixEnum::toType(polarizationString);
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
    test_ToString_<six::DualPolarizationType>("ToString", "V_V", six::DualPolarizationType::V_V);
    test_ToString_<six::DualPolarizationType>("ToString", "E_V", six::DualPolarizationType::E_V); // SICD 1.3
    test_ToString<six::DualPolarizationType>("ToString");

    test_ToString_<six::PolarizationType>("ToString", "V", six::PolarizationType::V);
    test_ToString_<six::PolarizationType>("ToString", "X", six::PolarizationType::X); // SICD 1.3
    test_ToString<six::PolarizationType>("ToString");

    test_ToString_<six::PolarizationSequenceType>("ToString", "V", six::PolarizationSequenceType::V);
    test_ToString_<six::PolarizationSequenceType>("ToString", "X", six::PolarizationSequenceType::X); // SICD 1.3
    test_ToString_<six::PolarizationSequenceType>("ToString", "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_ToString<six::PolarizationSequenceType>("ToString");
}

TEST_CASE(ToString_OTHER)
{
    const auto fromToType = six::PolarizationType::toType("OTHER_abc"); // SICD 1.3 "OTHER.*"   
    TEST_ASSERT_EQ(fromToType, six::PolarizationType::OTHER);
    TEST_ASSERT_EQ("OTHER_abc", fromToType.toString());
}

template<typename TSixEnum>
static void test_six_toString_(const std::string& testName, const std::string& strType, TSixEnum type)
{
    (void)testName;

    const std::string polarizationString = six::toString<TSixEnum>(type);
    TEST_ASSERT_EQ(strType, polarizationString);
    TEST_ASSERT_EQ(polarizationString, six::toString(type)); // no template parameter

    const auto pType = six::toType<TSixEnum>(polarizationString);
    TEST_ASSERT_EQ(pType, type);
}
template<typename TSixEnum>
static void test_six_toString(const std::string& testName)
{
    test_six_toString_<TSixEnum>(testName, "UNKNOWN", TSixEnum::UNKNOWN);
    test_six_toString_<TSixEnum>(testName, "OTHER", TSixEnum::OTHER);
}
TEST_CASE(SixToString)
{
    test_six_toString_<six::DualPolarizationType>("ToString", "V:V", six::DualPolarizationType::V_V);
    test_six_toString_<six::DualPolarizationType>("ToString", "E:V", six::DualPolarizationType::E_V); // SICD 1.3
    test_six_toString<six::DualPolarizationType>("SixToString");

    test_six_toString_<six::PolarizationType>("ToString", "V", six::PolarizationType::V);
    test_six_toString_<six::PolarizationType>("ToString", "X", six::PolarizationType::X); // SICD 1.3
    test_six_toString_<six::PolarizationType>("ToString", "OTHER", six::PolarizationType::OTHER);
    TEST_EXCEPTION(six::toString<six::PolarizationType>(six::PolarizationType::UNKNOWN)); // no conversion for six::PolarizationType::UNKNOWN

    test_six_toString_<six::PolarizationSequenceType>("ToString", "V", six::PolarizationSequenceType::V);
    test_six_toString_<six::PolarizationSequenceType>("ToString", "X", six::PolarizationSequenceType::X); // SICD 1.3
    test_six_toString_<six::PolarizationSequenceType>("ToString", "SEQUENCE", six::PolarizationSequenceType::SEQUENCE);
    test_six_toString<six::PolarizationSequenceType>("SixToString");
}

template<typename TSixEnum>
static void test_NotSet(const std::string& testName)
{
    (void)testName;

    TSixEnum pType;
    TEST_ASSERT_EQ(pType, TSixEnum::NOT_SET);
    std::string polarizationString = pType;
    TEST_ASSERT_EQ(polarizationString, "NOT_SET");
    TEST_EXCEPTION(polarizationString = pType.toString(true /*throw_if_not_set*/));
    polarizationString = pType.toString(false /*throw_if_not_set*/);
    TEST_ASSERT_EQ(polarizationString, "NOT_SET");

    pType = TSixEnum::toType("NOT SET");
    TEST_ASSERT_EQ(pType, TSixEnum::NOT_SET);
    pType = TSixEnum::toType("NOT_SET");
    TEST_ASSERT_EQ(pType, TSixEnum::NOT_SET);

    TEST_EXCEPTION(six::toType<TSixEnum>("NOT_SET"));

    TEST_EXCEPTION(six::toString(pType));
}
TEST_CASE(NotSet)
{
    test_NotSet< six::DualPolarizationType>("NotSet");
    test_NotSet< six::PolarizationType>("NotSet");
    test_NotSet< six::PolarizationSequenceType>("NotSet");
}

template<typename TSixEnum>
static void test_EqInt_(const std::string& testName, const std::string& strType, TSixEnum type, int enumValue)
{
    (void)testName;

    const decltype(type) fromStrCtor(strType);
    TEST_ASSERT_EQ(enumValue, fromStrCtor);
    const int value = fromStrCtor;
    TEST_ASSERT_EQ(enumValue, value);

    const decltype(type)fromIntCtor(value);
    TEST_ASSERT_EQ(enumValue, fromIntCtor);
    TEST_ASSERT(fromIntCtor == strType);
    TEST_ASSERT_EQ(fromIntCtor, type);

    TEST_ASSERT_EQ(fromStrCtor, fromIntCtor);
};
template<typename TSixEnum>
static void test_EqInt(const std::string& testName, int unknownEnumValue)
{
    test_EqInt_<TSixEnum>(testName, "UNKNOWN", TSixEnum::UNKNOWN, unknownEnumValue);
    test_EqInt_<TSixEnum>(testName, "OTHER", TSixEnum::OTHER, 1);
}
TEST_CASE(EqInt)
{
    test_EqInt_<six::DualPolarizationType>("EqInt", "V_V", six::DualPolarizationType::V_V, 2);
    test_EqInt_<six::DualPolarizationType>("EqInt", "E_V", six::DualPolarizationType::E_V, 56); // SICD 1.3
    test_EqInt< six::DualPolarizationType>("EqInt", 18);

    test_EqInt_<six::PolarizationType>("EqInt", "V", six::PolarizationType::V, 2);
    test_EqInt_<six::PolarizationType>("EqInt", "X", six::PolarizationType::X, 7); // SICD 1.3
    test_EqInt<six::PolarizationType>("EqInt", 6);

    test_EqInt_<six::PolarizationSequenceType>("EqInt", "V", six::PolarizationSequenceType::V, 2);
    test_EqInt_<six::PolarizationSequenceType>("EqInt", "X", six::PolarizationSequenceType::X, 8); // SICD 1.3
    test_EqInt_<six::PolarizationSequenceType>("EqInt", "SEQUENCE", six::PolarizationSequenceType::SEQUENCE, 7);
    test_EqInt<six::PolarizationSequenceType>("EqInt", 6);
}

TEST_CASE(DualPolarization)
{
    // https://pcf-om-mil-bb5cb050-f7c0-44fc-b114-b886abb80450.s3.us-east-1.amazonaws.com/doc/Document/NGA.STND.0024-1_1.3.0.pdf?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAWDCVGY5THCA2IKOI%2F20220531%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20220531T154720Z&X-Amz-Expires=900&X-Amz-SignedHeaders=host&response-cache-control=900&response-content-disposition=NGA.STND.0024-1_1.3.0.pdf&X-Amz-Signature=dc2de9c048244ce338da927ccaa26567ca8c5afc9c36aa384c21dc2e8faeec04
    // Allowed values include the form TX:RCV that is formed from one  TX value and one RCV value.
    // Allowed TX values: “V”, “H”, “X”, “Y”, “S”, “E”, “RHC”, “LHC”, “OTHER*”
    // Allowed RCV values:  “V”, “H”, “X”, “Y”, “S”, “E”, “RHC”, “LHC”, “OTHER*”,    

    const auto polarizationTypeMap = six::PolarizationType::string_to_int_();
    for (auto&& tx : polarizationTypeMap)
    {
        const auto txType = six::PolarizationType::toType(tx.first);
        if ((txType == six::PolarizationType::NOT_SET) || (txType == six::PolarizationType::UNKNOWN))
        {
            continue;
        }
        for (auto&& rcv : polarizationTypeMap)
        {
            const auto rcvType = six::PolarizationType::toType(rcv.first);
            if ((rcvType == six::PolarizationType::NOT_SET) || (rcvType == six::PolarizationType::UNKNOWN))
            {
                continue;
            }
            auto strType = txType.toString() + "_" + rcvType.toString();

            auto fromToType = six::DualPolarizationType::toType(strType);
            auto str = fromToType.toString();
            TEST_ASSERT_EQ(str, strType);
            test_toType_("DualPolarization", strType, fromToType);

            str::replace(strType, "_", ":");
            fromToType = six::toType<six::DualPolarizationType>(strType);
            str = six::toString(fromToType);
            TEST_ASSERT_EQ(str, strType);
            test_six_toType_("DualPolarization", strType, fromToType);
        }
    }
}

TEST_MAIN(
    TEST_CHECK(EnumConstructor);
    TEST_CHECK(ToType);
    TEST_CHECK(ToType_OTHER);
    TEST_CHECK(SixToType);
    TEST_CHECK(ToString);
    TEST_CHECK(ToString_OTHER);
    TEST_CHECK(SixToString);
    TEST_CHECK(NotSet);
    TEST_CHECK(EqInt);
    TEST_CHECK(DualPolarization);
    )
