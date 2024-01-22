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

#pragma once
#ifndef CODA_OSS_xml_lite_QName_h_INCLLUDED_
#define CODA_OSS_xml_lite_QName_h_INCLLUDED_

 /*!
 * \file QName.h
 * \brief A Qualified name (includes the namespace stuff)
 *
 * When XML started, the notion of a namespace was rarely used (and
 * many of us were happier that way).  Namespaces add complexity, but
 * the also allow for name resolution in a non-conflicting manner.
 * Unfortunately, the XML SAX readers, the event-driven parsers people
 * tend to use, at least as underlying structures have become more complex.
 * At any rate, a qualified name consists of 1) the local part (that is
 * the portion people used to use alone, without any namespace, and 2)
 * the prefix for the namespace, which presumably maps to a namespace URI
 *
 */

#include <string>
#include <ostream>

#include <config/Exports.h>

#include "sys/OS.h"
#include "str/Manip.h"

namespace xml
{

namespace lite
{
/*!
 *  \class QName
 *  \brief A Qualified name (includes the namespace stuff)
 *
 *  When XML started, the notion of a namespace was rarely used (and
 *  many of us were happier that way).  Namespaces add complexity, but
 *  the also allow for name resolution in a non-conflicting manner.
 *  Unfortunately, the XML SAX readers, the event-driven parsers people
 *  tend to use, at least as underlying structures have become more
 *  complex. At any rate, a qualified name consists of
 *  1) the local part (that is the portion people used to use alone,
 *  without any namespace, and
 *  2) the prefix for the namespace, which presumably maps
 *  to a namespace URI
 */

struct CODA_OSS_API Uri final  // help prevent mixups with std::string
{
    Uri();
    explicit Uri(const std::string& v);  // validate=false
    Uri(const std::string& v, bool validate);
    std::string value;
    bool empty() const
    {
        return value.empty();
    }
};
inline bool operator==(const Uri& lhs, const Uri& rhs)
{
    // URIs are "supposed to be" case-insenstive
    return str::eq(lhs.value, rhs.value);
}
inline bool operator!=(const Uri& lhs, const Uri& rhs)
{
    // URIs are "supposed to be" case-insenstive
    return str::ne(lhs.value, rhs.value);
}
inline std::ostream& operator<<(std::ostream& os, const Uri& uri)
{
    os << uri.value;
    return os;
}

class CODA_OSS_API QName final
{
    //!  Prefix (Qualified)
    std::string mPrefix;
    //!  Local Part (Unqualified)
    std::string mLocalName;
    //!  Associated URI for Prefix
    Uri mAssocUri;

public:
    QName() = default;

    /*!
     * Constructor taking the namespace prefix and the local name
     * \param uri The uri of the object
     * \param qname The qname of the object
     */
    QName(const xml::lite::Uri& uri, const std::string& qname)
    {
        setQName(qname);
        setAssociatedUri(uri);
    }
    QName(const std::string& qname, const xml::lite::Uri& uri) :
        QName(uri, qname)
    {
    }
    QName(const std::string& uri, const std::string& qname) :
        QName(Uri(uri), qname)
    {
    }

    /*!
     * Constructor taking just the local name (no namespace).
     * \param lName  Just the local name of the object.
     */
    explicit QName(const std::string& lName)
    {
        setName(lName);
    }

    QName(const xml::lite::Uri& uri)
    {
        setAssociatedUri(uri);
    }

    //! Destructor
    ~QName() = default;

    QName(const QName&) = default;
    QName& operator=(const QName&) = default;
    QName(QName&&) = default;
    QName& operator=(QName&&) = default;

    /*!
     *  Set the local part (unqualified)
     *  \param str The local name to set
     *
     */
    void setName(const std::string& str);
    std::string getName() const;
    /*!
     *  If this is a fully-qualified name, set it,
     *  otherwise, set the local name.
     *  \param str
     */
    void setQName(const std::string& str);

    /*!
     *  Set the prefix (name preceding colon)
     *  \param prefix A new prefix
     *
     */
    void setPrefix(const std::string& prefix);

    /*!
     *  Get the prefix
     *  \return The prefix
     *
     */
    std::string getPrefix() const;

    /*!
     *  Retrieve the qname as a string.  If you have no prefix/uri
     *  this returns just the local name
     *  \return The fully qualifed qname (e.g., soap-env:SOAP-BODY)
     */
    std::string toString() const;

    /*!
     *  A QName is nothing without its associated URI.
     *  Here you specify that URI.
     *  \param uri The URI to associate with this QName
     */
    void setAssociatedUri(const xml::lite::Uri&);
    void setAssociatedUri(const std::string& str)
    {
        setAssociatedUri(Uri(str));
    }

    /*!
     *  Get the URI associated with the QName
     *  \return The Associated URI
     *
     */
    std::string getAssociatedUri() const;
    void getAssociatedUri(xml::lite::Uri&) const;
    const xml::lite::Uri& getUri() const;
};
inline bool operator==(const QName& lhs, const QName& rhs)
{
    return (lhs.getName() == rhs.getName())
        && (lhs.getAssociatedUri() == rhs.getAssociatedUri())
        && (lhs.getPrefix() == rhs.getPrefix());
}
inline bool operator!=(const QName& lhs, const QName& rhs)
{
    return !(lhs == rhs);
}

namespace literals  // c.f. std::literals
{
    // This might be overkill, in part because we don't deal with a lot of literal QNames or URLs.
    // But it's interesting example code, and hidden in a "literals" namespace; be cautious
    // about extensive use outside of test code.

    // https://en.cppreference.com/w/cpp/language/user_literal
    inline xml::lite::Uri operator"" _u(const char* str, std::size_t len)
    {
        // https://en.cppreference.com/w/cpp/string/basic_string/operator%22%22s
        return xml::lite::Uri(std::string(str, len));
    }
    // https://en.cppreference.com/w/cpp/language/user_literal
    inline xml::lite::QName operator"" _q(const char* str, std::size_t len)
    {
        // https://en.cppreference.com/w/cpp/string/basic_string/operator%22%22s
        return xml::lite::QName(std::string(str, len));
    }
}

}
}
#endif  // CODA_OSS_xml_lite_QName_h_INCLLUDED_
