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

#ifndef __XML_LITE_QNAME_H__
#define __XML_LITE_QNAME_H__

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

class QName
{
public:
    //! Default constructor
    QName()
    {
    }

    /*!
     * Constructor taking the namespace prefix and the local name 
     * \param uri The uri of the object 
     * \param qname The qname of the object 
     */
    QName(const std::string& uri, const std::string& qname)
    {
        setQName(qname);
        setAssociatedUri(uri);
    }

    /*!
     * Constructor taking just the local name (no namespace). 
     * \param lName  Just the local name of the object. 
     */
    QName(const std::string& lName)
    {
        setName(lName);
    }

    //! Destructor
    ~QName()
    {
    }

    QName(const QName & qname)
    {
        mPrefix = qname.mPrefix;
        mLocalName = qname.mLocalName;
        mAssocUri = qname.mAssocUri;
    }

    QName& operator=(const QName& qname)
    {
        if (this != &qname)
        {
            mPrefix = qname.mPrefix;
            mLocalName = qname.mLocalName;
            mAssocUri = qname.mAssocUri;
        }
        return *this;
    }

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
    void setAssociatedUri(const std::string& uri);

    /*!
     *  Get the URI associated with the QName
     *  \return The Associated URI
     *
     */
    std::string getAssociatedUri() const;

protected:
    /*  Assignment operator  */
    QName& operator=(const std::string& str);

    //!  Prefix (Qualified)
    std::string mPrefix;
    //!  Local Part (Unqualified)
    std::string mLocalName;
    //!  Associated URI for Prefix
    std::string mAssocUri;

};
}
}

#endif
