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


#ifndef __DBI_PGSQLCONNECTION_H__
#define __DBI_PGSQLCONNECTION_H__

#if defined(USE_PGSQL)

#include "dbi/DatabaseConnection.h"
#include <libpq-fe.h>

/*!
 * \file PgSQLConnection.h
 * \brief PostgreSQL interface for database connections
 *
 */
namespace dbi
{
/*!
 *  \class PgSQLResultSet
 *  \brief This class defines the PostgreSQL interface for a result set
 */
class PgSQLResultSet : public ResultSet
{
public:
    /*!
    *  Default Constructor
    */
    PgSQLResultSet(PGresult* results) : ResultSet()
    {
        mResults = results; mRowIndex = 0;
    }

    /*!
     *  Default Destructor
     */
    ~PgSQLResultSet()
    {
        if (mResults) PQclear(mResults);
    }

    /*!
     *  fetchRow function returns the next row in the result set
     *  \return The next row
     */
    Row fetchRow();

    /*!
     *  getNumRows function returns the number of rows from the
     *  result set
     *  \return The number of Rows being returned
     */
    unsigned int getNumRows()
    {
        if (mResults)
            return PQntuples(mResults);
        return 0;
    }

private:
    PGresult* mResults;
    unsigned int mRowIndex;
};

/*!
 * \class PgSQLConnection
 * \brief PostgreSQL database interface
 *
 * This class provides the basis for PostgreSQL connections
 * 
 */
class PgSQLConnection : public DatabaseConnection
{
public:

    /*!
     *  Default Constructor
     */
    PgSQLConnection()
    {
        mDBHandle = nullptr;
    }

    /*!
     * Destructor
     *
     */
    ~PgSQLConnection()
    {}

    /*!
     *  Connect to the specified database
     *  \param database  The database name
            *  \param user  The username
            *  \param pass  The user password
     *  \param host  The computer host name where the database is located
     *  \param port  The receiving port on the host
     *  \return True if successful, False otherwise
     */
    bool connect(const std::string& database,
                 const std::string& user = "",
                 const std::string& pass = "",
                 const std::string& host = "localhost",
                 unsigned int port = 5432);

    /*!
     *  Disconnect connection to the database
     */
    void disconnect()
    {
        if (mDBHandle) PQfinish(mDBHandle);
    }

    /*!
     *  Send a command to the database as a string
     *  \param q  The command as a string
     *  \return The result set of command
            *  \throw SQLException on error
     */
    pResultSet query(const std::string& q);

    /*!
     *  Get the last connection error message
     *  \return The error message
     */
    const std::string getLastErrorMessage()
    {
        return PQerrorMessage(mDBHandle);
    }

private:
    /*! Handle to the PostgreSQL database connection */
    PGconn* mDBHandle;
};
}
#endif
#endif
