/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#ifndef __XML_LITE_ATTRIBUTES_H__
#define __XML_LITE_ATTRIBUTES_H__
#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "sys/Conf.h"
#include "except/Exception.h"
#include "xml/lite/QName.h"
#include "str/Convert.h"

/*!
 *  \file Attributes.h
 *  \brief Implementation of SAX 2.0 Attributes
 *
 *  This file contains all of the elements needed to approximate the
 *  SAX 2.0 standard for Attributes.  The implementation is simpler here,
 *  and not all interfaces are supported.  However, the functions match
 *  the Attributes interface from the specification
 */

namespace xml
{
namespace lite
{
/*!
 *  \class AttributeNode
 *  \brief An internal holding structure for Attributes.
 *
 *  This provides the definition of an Attribute's
 *  internal organs.  We have a URI, a QName, and a local part
 *  as well.  We also need a value, of course.
 */
class AttributeNode
{
public:

    //! Constructor
    AttributeNode()
    {
    }

    /*!
     *  Copy constructor
     *  \param attributeNode
     *  Copy attribute node to *this
     */
    AttributeNode(const AttributeNode & attributeNode);

    /*!
     *  Assignment operator
     *  \param attributeNode
     *  Copy attribute node to *this
     */
    AttributeNode& operator=(const AttributeNode& attributeNode);

    //! Destructor
    ~AttributeNode()
    {
    }

    /*!
     *  This function takes a fully qualified name.
     *  It does not allow for a URI.  use setUri() to
     *  bind a namespace
     *  \param qname The fully qualified name
     */
    void setQName(const std::string& qname);

    /*!
     *  Set the local (unqualified portion) of the name
     *  \param lname The new local name
     *
     */
    void setLocalName(const std::string& lname);

    /*!
     *  Set the prefix (qualified portion) of the name
     *  \param prefix  This is the prefix
     */
    void setPrefix(const std::string& prefix);

    /*!
     *  Set the URI association in the QName
     *  \param uri The new uri
     */
    void setUri(const std::string& uri);

    /*!
     *  Set the attribute value
     *  \param value The attribute value
     */
    void setValue(const std::string& value);

    /*!
     *  Get the URI associated with the QName.  Blank string
     *  if none.
     *  \return The uri
     */
    std::string getUri() const;
    std::string getLocalName() const;
    std::string getPrefix() const;
    std::string getValue() const;
    std::string& getValue()
    {
        return mValue;
    }
    std::string getQName() const;

protected:

    QName mName;
    //! The value of the attribute
    std::string mValue;

};

/*!
 *  \class Attributes
 *  \brief This class handles the attribute components.
 *
 *  The DOM and SAX specifications are very different,
 *  but their difference is what makes the implementations so
 *  onerous.  Here, we simplify the interface so that it dom and
 *  sax are essentially sharing the same components.  The dom
 *  model is built on the sax model, allowing us to make use of
 *  this data structure everywhere.  That also allows us to
 *  simplify future dom classes
 */
class Attributes
{
public:

    typedef std::vector<AttributeNode> Attributes_T;
    //! Default constructor
    Attributes()
    {
    }

    //! Copy constructor
    Attributes(const Attributes & attributes);

    //! Assignment operator
    Attributes & operator=(const Attributes & attributes);

    //! Destructor
    ~Attributes()
    {
    }

    /*!
     *  Adds an attribute to the list of attributes.
     *  \param attribute the attribute to add
     */
    AttributeNode& add(const AttributeNode& attribute);

    /*!
     * Look up the index of an index.
     * \param i  The index of the attribute
     * \return the index or -1 if none found
     */
    int getIndex(int i) const;

    /*!
     * Look up the index of an attribute by XML 1.0 qualified name.
     * \param qname The fully qualified name of the attribute
     * \return the index or -1 if none found
     */
    int getIndex(const std::string & qname) const;

    /*!
     * Look up the index of an attribute by Namespace name.
     * \param uri  The uri association
     * \param localName  The local name of the attribute
     * \return the index or -1 if none found
     */
    int getIndex(const std::string & uri, const std::string & localName) const;
    int getIndex(const std::tuple<std::string, std::string>& name) const
    {
        return getIndex(std::get<0>(name), std::get<1>(name));
    }

    /*!
     * Return the number of attributes in the list.
     * \return The number of attributes contained herein
     */
    int getLength() const
    {
        return (int) mAttributes.size();
    }

    /*!
     * Look up an attribute's local name by index.
     * \param i  The index of the attribute
     * \return The local name of the attribute
     * \throw IndexOutOfRangeException if the index is out of range
     */
    std::string getLocalName(int i) const;

    std::string getQName(int i) const;

    /*!
     * Look up an attribute's Namespace URI by index.
     * \param i The index for the attribute we want
     * \return the uri for our attribute
     * \throw IndexOutOfRangeException if the index is out of range
     */
    std::string getUri(int i) const;

    /*!
     * Look up an attribute's value by index.
     * \param i  The index for the attribute we want
     * \return the value of the attribute
     * \throw IndexOutOfRangeException if the index is out of range
     */
    std::string getValue(int i) const;
    /*!
     * Look up an attribute's value by index.
     * \param i  The index for the attribute we want
     * \param result The value, if found
     * \return If the index is out of range or not
     */
    bool getValue(int i, std::string& result) const;

    /*!
     * Look up an attribute's value by XML 1.0 qualified name.
     * \param qname The qualified name
     * \return The value
     * \throw NoSuchKeyException If the qname is not found
     */
    std::string getValue(const std::string & qname) const;
    /*!
     * Look up an attribute's value by XML 1.0 qualified name.
     * \param qname The qualified name
     * \param result The value, if found
     * \return If the qname is not found or not
     */
    bool getValue(const std::string& qname, std::string& result) const;

    /*!
     * Look up an attribute's value by Namespace name.
     * \param uri The uri association
     * \param localName the local name of the object
     * \return The value
     * \throw NoSuchKeyException If the uri/localName is not found
     */
    std::string getValue(const std::string & uri, const std::string & localName) const;
    std::string getValue(const std::tuple<std::string, std::string>& name) const
    {
        return getValue(std::get<0>(name), std::get<1>(name));
    }

    /*!
     * Look up an attribute's value by Namespace name.
     * \param uri The uri association
     * \param localName the local name of the object
     * \param result The value, if found
     * \return If the uri/localName is not found or not
     */
    bool getValue(const std::string& uri, const std::string& localName, std::string& result) const;
    bool getValue(const std::tuple<std::string, std::string>& name, std::string& result) const
    {
        return getValue(std::get<0>(name), std::get<1>(name), result);
    }

    /*!
     * Get an attribute note based on the index as a const ref
     * \param i The node index
     * \return The attribute node
     */
    const AttributeNode& getNode(int i) const;

    /*!
     * Get an attribute note based on the index as a non-const ref
     * \param i The node index
     * \return The attribute node
     */
    AttributeNode& getNode(int i);

    /*!
     * Get an attribute note based on the index as a const ref
     * \param i The node index
     * \return The attribute node
     */

    const AttributeNode& operator[](int i) const
    {
        return getNode(i);
    }

    std::string& operator[](const std::string& s)
    {
        int idx = getIndex(s);
        if (idx < 0)
        {
            AttributeNode node;
            node.setQName(s);
            mAttributes.push_back(node);
            idx = (int)(mAttributes.size() - 1);
        }
        return mAttributes[(size_t)idx].getValue();
    }

    /*!
     * Get an attribute note based on the index as a non-const ref
     * \param i The node index
     * \return The attribute node
     */
    AttributeNode& operator[](int i)
    {
        return getNode(i);
    }

    /**
     * Remove the attribute with the given QName
     */
    void remove(const std::string& qname)
    {
        int idx = getIndex(qname);
        if (idx >= 0)
            remove((size_t)idx);
    }

    bool contains(const std::string& qname) const
    {
        try
        {
            getValue(qname);
            return true;
        }
        catch(const except::NoSuchKeyException&)
        {
            return false;
        }
    }

    /**
     * Remove the attribute at the given index (if possible)
     */
    void remove(size_t index)
    {
        if (index < mAttributes.size())
        {
            Attributes_T::iterator it = mAttributes.begin();
            for(size_t i = 0; i < index; ++i, ++it);
            mAttributes.erase(it);
        }
    }

    void clear()
    {
        mAttributes.clear();
    }
private:
    //! Underlying representation
    Attributes_T mAttributes;
};

#ifndef SWIG
// The (old) version of SWIG we're using doesn't like certain C++11 features.

/*!
 * Look up an attribute's value by an arbitrary key.
 * \param key  The key for the attribute we want
 * \param result The value after calling str::toType(), if found
 * \return If an attribute with the key is found or not
 */
template <typename K, typename ToType>
inline auto castValue_(const Attributes& attributes, const K& key, ToType toType)
  -> decltype(toType(std::string())) 
{
    const auto value = attributes.getValue(key);
    if (value.empty())
    {
        throw except::BadCastException(Ctxt("call Attributes::getValue() directly to get an empty string"));
    }
    return toType(value);
}
template <typename T, typename K, typename ToType>
inline bool castValue_(const Attributes& attributes, const K& key, T& result, ToType toType)
{
    try
    {
        result = castValue_(attributes, key, toType);
    }
    catch (const except::NoSuchKeyException&)
    {
        return false;
    }
    catch (const except::BadCastException&)
    {
        return false;
    }
    return true;
}
namespace details
{
template <typename T>
inline T toType(const std::string& value)
{
    return str::toType<T>(value);
}
}

/*!
 * Look up an attribute's value by index.
 * \param i  The index for the attribute we want
 * \param result The value after calling str::toType(), if found
 * \return If the index is out of range or not
 */
/*!
 * Look up an attribute's value by XML 1.0 qualified name.
 * \param qname The qualified name
 * \param result The value after calling str::toType(), if found
 * \return If the qname is not found or not
 */
template <typename ToType, typename TKey>
inline auto castValue(const Attributes& attributes, const TKey& k, ToType toType)
  -> decltype(toType(std::string()))
{
    return castValue_(attributes, k, toType);
}
template <typename T, typename TKey>
inline T getValue(const Attributes& attributes, const TKey& k)
{
    return castValue(attributes, k, details::toType<T>);
}

template <typename T, typename ToType, typename TKey>
inline bool castValue(const Attributes& attributes, const TKey& k, T& result, ToType toType)
{
    return castValue_(attributes, k, result, toType);
}
template <typename T, typename TKey>
inline bool getValue(const Attributes& attributes, const TKey& k, T& result)
{
    return castValue(attributes, k, result, details::toType<T>);
}

/*!
 * Look up an attribute's value by Namespace name.
 * \param uri The uri association
 * \param localName the local name of the object
 * \param result The value after calling str::toType(), if found
 * \return If the uri/localName is not found or not
 */
template <typename ToType>
inline auto castValue(const Attributes& attributes, const std::string & uri, const std::string & localName, ToType toType)
-> decltype(toType(std::string()))
{
    return castValue(attributes, std::make_tuple(uri, localName), toType);
}
template <typename T>
inline T getValue(const Attributes& attributes, const std::string & uri, const std::string & localName)
{
    return getValue<T>(attributes, std::make_tuple(uri, localName));
}

template <typename T, typename ToType>
inline bool castValue(const Attributes& attributes, const std::string & uri, const std::string & localName, T& result, ToType toType)
{
    return getValue(attributes, std::make_tuple(uri, localName), result, toType);
}
template <typename T>
inline bool getValue(const Attributes& attributes, const std::string & uri, const std::string & localName, T& result)
{
    return getValue(attributes, std::make_tuple(uri, localName), result);
}

/*!
 * Set an attribute's value with an arbitrary key.
 * \param key  The key for the attribute we want
 * \param value The value to be converted by calling str::toString
 * \return If an attribute with the key is found or not
 */
template <typename T, typename K, typename ToString>
inline bool setValue_(Attributes& attributes, const K& key, const T& value,
    ToString toString)
{
    int index = attributes.getIndex(key);
    if (index < 0)
    {
        return false; // not found
    }

    auto& node = attributes.getNode(index);
    node.setValue(toString(value));
    return true;
}
namespace details
{
template <typename T>
inline std::string toString(const T& value)
{
    return str::toString(value);
}
}

/*!
 * Look up an attribute's value by index.
 * \param i  The index for the attribute we want
 * \param result The value after calling str::toType(), if found
 * \return If the index is out of range or not
 */
template <typename T, typename ToString>
inline bool setValue(Attributes& attributes, int i, const T& value, ToString toString)
{
    return setValue_(attributes, i, value, toString);
}
template <typename T>
inline bool setValue(Attributes& attributes, int i, const T& value)
{
    return setValue_(attributes, i, value, details::toString<T>);
}

/*!
 * Look up an attribute's value by XML 1.0 qualified name.
 * \param qname The qualified name
 * \param result The value after calling str::toType(), if found
 * \return If the qname is not found or not
 */
template <typename T, typename ToString>
inline bool setValue(Attributes& attributes, const std::string& qname, const T& value,
    ToString toString)
{
    return setValue_(attributes, qname, value, toString);
}
template <typename T>
inline bool setValue(Attributes& attributes, const std::string& qname, const T& value)
{
    return setValue_(attributes, qname, value, details::toString<T>);
}

/*!
 * Look up an attribute's value by Namespace name.
 * \param uri The uri association
 * \param localName the local name of the object
 * \param result The value after calling str::toType(), if found
 * \return If the uri/localName is not found or not
 */
template <typename T, typename ToString>
inline bool setValue(Attributes& attributes, const std::tuple<std::string, std::string>& name, const T& value,
        ToString toString)
{
    return setValue_(attributes, name, value, toString);
}
template <typename T>
inline bool setValue(Attributes& attributes, const std::tuple<std::string, std::string>& name, const T& value)
{
    return setValue_(attributes, name, value, details::toString<T>);
}
template <typename T, typename ToString>
inline bool setValue(Attributes& attributes, const std::string & uri, const std::string & localName, const T& value,
     ToString toString)
{
    return setValue(attributes, std::make_tuple(uri, localName), value, toString);
}
template <typename T>
inline bool setValue(Attributes& attributes, const std::string & uri, const std::string & localName, const T& value)
{
    return setValue(attributes, uri, localName, value, details::toString<T>);
}
#endif // SWIG

}
}

#endif
