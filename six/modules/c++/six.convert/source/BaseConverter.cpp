/* =========================================================================
 * This file is part of six.convert-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.convert-c++ is free software; you can redistribute it and/or modify
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
#include "six/convert/BaseConverter.h"
#include <io/FileInputStream.h>
#include <xml/lite/MinidomParser.h>
#include <iostream>

namespace six
{
namespace convert
{
BaseConverter::BaseConverter() :
    six::XMLParser("", false)
{
}

mem::auto_ptr<xml_lite::Document>
BaseConverter::readXML(const std::string& xmlPathname)
{
    xml_lite::MinidomParser parser;
    io::FileInputStream xmlInputStream(xmlPathname);
    parser.parse(xmlInputStream);
    return mem::auto_ptr<xml_lite::Document>(
            parser.getDocument(true));
}

BaseConverter::XMLElem BaseConverter::findUniqueElement(
        const xml_lite::Element* root, const std::string& xmlPath) const
{
    const xml_lite::Element* node = root;
    const std::vector<std::string> nodes = str::split(xmlPath, "/");
    for (size_t ii = 0; ii < nodes.size(); ++ii)
    {
        node = getFirstAndOnly(node, nodes[ii]);
    }
    return const_cast<XMLElem>(node);
}

std::vector<BaseConverter::XMLElem> BaseConverter::findElements(
        const xml_lite::Element* root, const std::string& xmlPath) const
{
    const xml_lite::Element* node = root;
    const std::vector<std::string> nodes = str::split(xmlPath, "/");
    if (nodes.empty())
    {
        throw except::Exception(Ctxt("str::split(" + xmlPath +
                ") returned an empty vector!"));
    }
    for (size_t ii = 0; ii < nodes.size() - 1; ++ii)
    {
        node = getFirstAndOnly(node, nodes[ii]);
    }
    return node->getElementsByTagName(nodes[nodes.size() - 1]);
}

std::string BaseConverter::parseStringFromPath(const xml_lite::Element* root,
        const std::string& xmlPath) const
{
    std::string parsedString;
    const xml_lite::Element* target = findUniqueElement(root, xmlPath);
    parseString(target, parsedString);
    return parsedString;
}

double BaseConverter::parseDoubleFromPath(const xml_lite::Element* root,
        const std::string& xmlPath) const
{
    double parsedDouble;
    const xml_lite::Element* target = findUniqueElement(root, xmlPath);
    parseDouble(target, parsedDouble);
    return parsedDouble;
}

size_t BaseConverter::parseUIntFromPath(const xml_lite::Element* root,
        const std::string& xmlPath) const
{
    size_t parsedUInt = 0;
    const xml_lite::Element* target = findUniqueElement(root, xmlPath);
    parseUInt(target, parsedUInt);
    return parsedUInt;
}

six::DateTime BaseConverter::parseDateTimeFromPath(const xml_lite::Element* root,
        const std::string& xmlPath) const
{
    six::DateTime parsedDateTime;
    const xml_lite::Element* target = findUniqueElement(root, xmlPath);
    parseDateTime(target, parsedDateTime);
    return parsedDateTime;
}

size_t BaseConverter::findIndexByAttribute(const std::vector<XMLElem>& elements,
        const std::string& attribute, const std::string& value) const
{
    const std::vector<std::string> attributes(1, attribute);
    const std::vector<std::string> values(1, value);

    return findIndexByAttributes(elements, attributes, values);
}

size_t BaseConverter::findIndexByAttributes(const std::vector<XMLElem>& elements,
        const std::vector<std::string>& attributes,
        const std::vector<std::string>& values) const
{
    for (size_t ii = 0; ii < elements.size(); ++ii)
    {
        xml_lite::Element* const element = elements[ii];
        bool doAttributesMatch = true;
        for (size_t jj = 0; jj < attributes.size(); ++jj)
        {
            if (element->attribute(attributes[jj]) != values[jj])
            {
                doAttributesMatch = false;
                break;
            }
        }
        if (doAttributesMatch)
        {
            return ii;
        }
    }

    throw except::Exception(Ctxt("Unable to find element"));
}

size_t BaseConverter::findIndex(const std::vector<XMLElem>& elements,
        const std::string& xmlPath, const std::string& value) const
{
    for (size_t ii = 0; ii < elements.size(); ++ii)
    {
        const std::string element = parseStringFromPath(elements[ii], xmlPath);
        if (element == value)
        {
            return ii;
        }
    }
    throw except::Exception(Ctxt("Unable to find element with value " + value));
}

double BaseConverter::sumOverElements(const std::vector<XMLElem>& elements,
        const std::string& xmlPath) const
{
    double sum = 0;
    for (size_t ii = 0; ii < elements.size(); ++ii)
    {
        sum += parseDoubleFromPath(elements[ii], xmlPath);
    }
    return sum;
}
}
}

