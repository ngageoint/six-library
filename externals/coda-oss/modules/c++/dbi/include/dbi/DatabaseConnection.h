/* =========================================================================
 * This file is part of dbi-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * dbi-c++ is free software; you can redistribute it and/or modify
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


#ifndef __DBI_DATABASECONNECTION_H__
#define __DBI_DATABASECONNECTION_H__
#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include "except/Exception.h"
#include "str/Convert.h"
#include "sys/Conf.h"
#include "mem/SharedPtr.h"

/*!
 * \file DatabaseConnection.h
 * \brief Interface for database connections
 *
 */

namespace dbi
{
/*!
 *  \class Field
 *  \brief This class defines a field in a given row from 
 *   the result set
 *
 */
class Field
{
public:
    /*!
     *  Default Constructor 
     *  \param name  Field name
     *  \param type  Field type (e.g. int, text, etc.)
     *  \param size  Field size
            *  \param data  Field data
     */
    template <typename T>
    Field(const std::string& name, int type, unsigned int size, T data) :
            mName(name), mType(type), mSize(size)
    {
        mData = str::toString(data);
    }

    /*!
     *  Default Destructor
     */
    ~Field()
    {}

    /*!
     *  Function for extracting name from this storage class
     *  \return The field name
     */
    const std::string& getName() const
    {
        return mName;
    }

    /*!
     *  Function for extracting name from this storage class
     *  \return The field type
     */
    int getType() const
    {
        return mType;
    }

    /*!
     *  Function for extracting name from this storage class
     *  \return The field size
     */
    unsigned int getSize() const
    {
        return mSize;
    }

    /*!
     *  Function for extracting data from this storage class
     *  \return The field data
     */
    template <typename T>
    T getData() const
    {
        return str::toType<T>(mData);
    }

protected:
    /*! Protected field name */
    std::string mName;

    /*! Protected field type */
    int mType;

    /*! Protected field size */
    unsigned int mSize;

    /*! Protected field data */
    std::string mData;
};

/*!
 *  \class Row
 *  \brief This class defines a row in a result set
 *  
 */
class Row
{
public:
    /*!
    *  Default Constructor
    */
    Row() : mNumFields(0)
    {}

    /*!
     *  Default Destructor
     */
    ~Row()
    {}

    /*!
     *  Overloaded [] operator function returns the actual row cell data,
     *  but prevents it from being modified
     *  \param field The field name that maps to an index
     *  \return The actual field data of this row
     */
    Field& operator[] (const std::string& field)
    {
        return mData[mFieldIndex[field]];
    }

    /*!
     *  Function for extracting field type from this row
     *  \param field The field name that maps to an index
     *  \return The actual field type
     */
    int getFieldType(const std::string& field)
    {
        return mData[mFieldIndex[field]].getType();
    }

    /*!
     *  Function for extracting field size from this row
     *  \param field The field name that maps to an index
     *  \return The actual field size
     */
    unsigned int getFieldSize(const std::string& field)
    {
        return mData[mFieldIndex[field]].getSize();
    }

    /*!
     *  Overloaded [] operator function returns the actual row cell data,
     *  but prevents it from being modified
     *  \param index The index of the field
     *  \return The actual field data of this row
     */
    Field& operator[] (int index)
    {
        return mData[index];
    }

    /*!
     *  Function for extracting field name from this row
     *  \param index The index of the field
     *  \return The actual field name
     */
    const std::string& getFieldName(int index)
    {
        return mData[index].getName();
    }

    /*!
     *  Function for extracting field type from this row
     *  \param index The index of the field
     *  \return The actual field type
     */
    int getFieldType(int index)
    {
        return mData[index].getType();
    }

    /*!
     *  Function for extracting field size from this row
     *  \param index The index of the field
     *  \return The actual field size
     */
    unsigned int getFieldSize(int index)
    {
        return mData[index].getSize();
    }

    /*!
     *  Function adding a field to this row
     *  \param name The name of the field to be added
     *  \param type The type of the field to be added
     *  \param size The size of the field to be added
     *  \param data The data of the field to be added
     */
    template <typename T>
    void addField(const std::string& name,
                  int type,
                  int size,
                  T data)
    {
        mData.push_back(Field(name, type, size, data));
        mFieldIndex[name] = mNumFields++;
    }

    /*!
     *  Function gets number of fields for this row
     *  \return The number of fields for this row
     */
    int getNumFields()
    {
        return mNumFields;
    }

    /*!
     *  Function clears row data
     */
    void clear()
    {
        mNumFields = 0;
        mFieldIndex.clear();
        mData.clear();
    }

protected:
    /*! Index that gives the position of the next field added */
    int mNumFields;

    /*! Maps a field name to the field index for this row */
    std::map< std::string, int > mFieldIndex;

    /*! Holds the fields for this row */
    std::vector< Field > mData;
};

//typedef std::auto_ptr< Row > pRow;

/*!
 *  \class ResultSet
 *  \brief This class defines the abstract interface for a result set
 * 
 *  An abstract result set is the base from which all result sets are
 *  derived.  It provides a common interface.
 */
class ResultSet
{
public:
    /*!
    *  Default Constructor
    */
    ResultSet()
    {}

    /*!
     *  Default Destructor
     */
    virtual ~ResultSet()
    {}

    /*!
     *  fetchRow function returns the next row in the result set
     *  \return The next row
     */
    virtual Row fetchRow() = 0;

    /*!
     *  getNumRows function returns the number of rows from the
     *  result set
     *  \return The number of Rows being returned
     */
    virtual unsigned int getNumRows() = 0;

protected:
    /*! The most recently fetched row */
    Row mCurrentRow;
};

typedef mem::auto_ptr< ResultSet > pResultSet;

/*!
 * \class DatabaseConnection
 * \brief Abstract database interface
 *
 * This class provides the base interface for database connections
 * 
 */
class DatabaseConnection
{
public:

    /*!
     *  Default Constructor
     */
    DatabaseConnection()
    {}

    /*!
     * Destructor
     *
     */
    virtual ~DatabaseConnection()
    {}

    /*!
     *  Connect to the specified database
     *  \param database  The database name
     *  \param host  The computer host name where the database is located
     *  \param port  The receiving port on the host
     *  \param user  The username
     *  \param pass  The user password
     *  \return True if successful, False otherwise
     */
    virtual bool connect(const std::string& database,
                         const std::string& user = "",
                         const std::string& pass = "",
                         const std::string& host = "localhost",
                         unsigned int port = 0) = 0;
    /*!
     *  Disconnect connection to the database
     */
    virtual void disconnect() = 0;

    /*!
     *  Send a command to the database as a string
     *  \param q  The command as a string
     *  \return The result set of command
            *  \throw SQLException on error
     */
    virtual pResultSet query(const std::string& q) = 0;

    /*!
     *  Get the last connection error message
     *  \return The error message
     */
    virtual const std::string getLastErrorMessage() = 0;

};

/*!
 * \class SQLException
 * \brief This is responsible for handling exceptions for this API
 * 
 */
class SQLException : public except::Exception
{
public:
    /*!
     *  Default Constructor
     */
    SQLException()
    {}

    /*!
     *  Construct from context
     *  \param c The exception context
     */
    SQLException(const except::Context& c) :
            except::Exception(c)
    {}

    /*!
     *  Construct from message
     *  \param message The exception message
     */
    SQLException(const std::string& message) :
            except::Exception(message)
    {}

    /*!
     * User constructor. Takes an Throwable and a Context
     * \param t The Throwable
     * \param c The Context
     */
    SQLException(const except::Throwable& t, const except::Context& c) :
            except::Exception(t, c)
    {}

    /*!
     *  Destructor
     */
    ~SQLException()
    {}
};
}
#endif
