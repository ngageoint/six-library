/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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

#include <std/filesystem>

#include <six/XMLControl.h>
#include <string>
#include <vector>

#include "six/XmlLite.h"

#include "TestCase.h"

// prefer SIX_DEFAULT_SCHEMA_PATH, existing scripts use DEFAULT_SCHEMA_PATH
#if defined(DEFAULT_SCHEMA_PATH) && !defined(SIX_DEFAULT_SCHEMA_PATH)
#define SIX_DEFAULT_SCHEMA_PATH DEFAULT_SCHEMA_PATH 
#endif
#ifndef SIX_DEFAULT_SCHEMA_PATH
// Don't want to set a dummy schema path to a directory that exists as that causes
// the code to check for valid schemas and validate.
#if defined(_WIN32)
#define SIX_DEFAULT_SCHEMA_PATH "Z:\\s 0 m e\\p at h" // just to compile ...
#else
#define SIX_DEFAULT_SCHEMA_PATH "/s 0 m e/p at h" // just to compile ...
#endif
#endif

TEST_CASE(loadCompiledSchemaPath)
{
    sys::OS().unsetEnv("SIX_SCHEMA_PATH");
    std::vector<std::string> schemaPaths;
    six::XMLControl::loadSchemaPaths(schemaPaths);

    size_t schemaPathsSize = 0;
    if (std::filesystem::exists(SIX_DEFAULT_SCHEMA_PATH))
    {
        schemaPathsSize++;
    }
    TEST_ASSERT_EQ(schemaPaths.size(), schemaPathsSize);
    if (!schemaPaths.empty())
    {
        TEST_ASSERT_EQ(schemaPaths[0], SIX_DEFAULT_SCHEMA_PATH);
    }
}

TEST_CASE(respectGivenPaths)
{
    std::vector<std::string> schemaPaths = {"some/path"};
    sys::OS().setEnv("SIX_SCHEMA_PATH", "another/path", true /*overwrite*/);
    six::XMLControl::loadSchemaPaths(schemaPaths);
    TEST_ASSERT_EQ(schemaPaths.size(), static_cast<size_t>(1));
}

TEST_CASE(loadFromEnvVariable)
{
    std::vector<std::string> schemaPaths;
    sys::OS().setEnv("SIX_SCHEMA_PATH", "another/path", true /*overwrite*/);
    six::XMLControl::loadSchemaPaths(schemaPaths);
    TEST_ASSERT_EQ(schemaPaths.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(schemaPaths[0], "another/path");
}

TEST_CASE(ignoreEmptyEnvVariable)
{
    std::vector<std::string> schemaPaths;
    sys::OS().setEnv("SIX_SCHEMA_PATH", "   ", true /*overwrite*/);
    six::XMLControl::loadSchemaPaths(schemaPaths);

    size_t schemaPathsSize = 0;
    if (std::filesystem::exists(SIX_DEFAULT_SCHEMA_PATH))
    {
        schemaPathsSize++;
    }
    TEST_ASSERT_EQ(schemaPaths.size(), schemaPathsSize);
    if (!schemaPaths.empty())
    {
        TEST_ASSERT_EQ(schemaPaths[0], SIX_DEFAULT_SCHEMA_PATH);
    }
}

TEST_CASE(dataTypeToString)
{
    std::string result = six::XMLControl::dataTypeToString(six::DataType::COMPLEX);
    TEST_ASSERT_EQ("SICD_XML", result);

    result = six::XMLControl::dataTypeToString(six::DataType::DERIVED, false /*appendXML*/);
    TEST_ASSERT_EQ("SIDD", result);

    // Generate a garbage value to test the exception.  Have to hack-things-up
    // because there are overloads on six::DataType for integer assignment.
    auto dataType = six::DataType::COMPLEX;
    void* pDataType = &dataType;
    int* pIntDataType = static_cast<int*>(pDataType);
    *pIntDataType = 999; // bypass overloads; should now be a garbage value
    TEST_ASSERT_NOT_EQ(dataType, six::DataType::COMPLEX);
    TEST_ASSERT_NOT_EQ(dataType, six::DataType::DERIVED);
    TEST_EXCEPTION(six::XMLControl::dataTypeToString(dataType)); // the "default:" case label will throw, as desired
}

TEST_CASE(testXmlLiteAttributeClass)
{
    six::XmlLite xmlLite(xml::lite::Uri("urn:example.com"), true /*addClassAttributes*/);
    auto root = xmlLite.newElement("root", nullptr /*prnt*/);

    {
        auto& e = xmlLite.createDouble("double", 3.14, *root);
        const auto& attrib = e.attribute("class");
        TEST_ASSERT_EQ(attrib, "xs:double");
    }
    {
        auto& e = xmlLite.createInt("int", 314, *root);
        const auto& attrib = e.attribute("class");
        TEST_ASSERT_EQ(attrib, "xs:int");
    }
    {
        auto& e = xmlLite.createString("string", "abc", *root);
        const auto& attrib = e.attribute("class");
        TEST_ASSERT_EQ(attrib, "xs:string");
    }
    {
        auto* e = xmlLite.createBooleanType(xml::lite::QName("Boolean"), six::BooleanType::IS_TRUE, *root);
        const auto& attrib = e->attribute("class");
        TEST_ASSERT_EQ(attrib, "xs:boolean");
    }

    // TODO: xs:date, xs:dateTime
}

template<typename T>
void test_six_toString_Exception(const std::string& testName)
{
    auto v = six::Enum::cast<T>(1); // most enums have a value for 1

    const auto s1 = v.toString(); // uses "int"
    const auto s2 = six::Enum::toString(v); // uses T::values
    TEST_ASSERT_EQ(s1, s2);
    const auto s3 = six::toString(v);
    TEST_ASSERT_EQ(s1, s3);

    v.set_value_(-2); // cause toString() to fail
    TEST_EXCEPTION(six::toString(v));
    v = T::NOT_SET;
    TEST_EXCEPTION(six::toString(v)); // NOT_SET throws for this type
}
TEST_CASE(test_six_toString)
{
    // This doesn't have anything to do with XML per-se, but that's the main use-case for six::toString()

    test_six_toString_Exception<six::MagnificationMethod>(testName);
    test_six_toString_Exception<six::DecimationMethod>(testName);
    test_six_toString_Exception<six::OrientationType>(testName);
    test_six_toString_Exception<six::DemodType>(testName);
    test_six_toString_Exception<six::ImageFormationType>(testName);
    test_six_toString_Exception<six::SlowTimeBeamCompensationType>(testName);
    test_six_toString_Exception<six::ImageBeamCompensationType>(testName);
    test_six_toString_Exception<six::AutofocusType>(testName);
    test_six_toString_Exception<six::RMAlgoType>(testName);
    test_six_toString_Exception<six::ComplexImagePlaneType>(testName);
    test_six_toString_Exception<six::ComplexImageGridType>(testName);
    test_six_toString_Exception<six::CollectType>(testName);
    test_six_toString_Exception<six::RadarModeType>(testName);
}

template<typename T>
void test_six_toType_(const std::string& testName)
{
    const auto v = six::Enum::cast<T>(1); // most enums have a value for 1
    const auto s = six::toString(v);
    auto v_ = six::toType<T>(s);
    TEST_ASSERT_EQ(v_, v);
    v_ = six::toType<T>(" " + s + " "); // code should call str::trim()
    TEST_ASSERT_EQ(v, v_);
}
template<typename T>
void test_six_toType_NOT_SET(const std::string& testName)
{
    test_six_toType_<T>(testName);
    const auto v = six::toType<T>("Q W E R T Y"); // any string that will cause failure
    TEST_ASSERT_EQ(v, T::NOT_SET); // returns NOT_SET rather than throwing
}
template<typename T>
void test_six_toType_Exception(const std::string& testName)
{
    test_six_toType_<T>(testName);
    TEST_EXCEPTION(six::toType<T>("Q W E R T Y" /*any string that will cause failure*/)); // throw rather than returning NOT_SET
}
TEST_CASE(test_six_toType)
{
    // This doesn't have anything to do with XML per-se, but that's the main use-case for six::toString()

    test_six_toType_NOT_SET<six::MagnificationMethod>(testName);
    test_six_toType_NOT_SET<six::DecimationMethod>(testName);
    test_six_toType_Exception<six::OrientationType>(testName);
    test_six_toType_Exception<six::DemodType>(testName);
    test_six_toType_Exception<six::ImageFormationType>(testName);
    test_six_toType_Exception<six::SlowTimeBeamCompensationType>(testName);
    test_six_toType_Exception<six::ImageBeamCompensationType>(testName);
    test_six_toType_Exception<six::AutofocusType>(testName);
    test_six_toType_Exception<six::RMAlgoType>(testName);
    test_six_toType_Exception<six::ComplexImagePlaneType>(testName);
    test_six_toType_Exception<six::ComplexImageGridType>(testName);
    test_six_toType_Exception<six::CollectType>(testName);
    test_six_toType_NOT_SET<six::RadarModeType>(testName);

}

TEST_MAIN(
    TEST_CHECK(loadCompiledSchemaPath);
    TEST_CHECK(respectGivenPaths);
    TEST_CHECK(loadFromEnvVariable);
    TEST_CHECK(ignoreEmptyEnvVariable);
    TEST_CHECK(dataTypeToString);
    TEST_CHECK(testXmlLiteAttributeClass);

    TEST_CHECK(test_six_toString);
    TEST_CHECK(test_six_toType);
    )
