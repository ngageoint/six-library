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

#ifndef __DBI_ORACLECONNECTION_H__
#define __DBI_ORACLECONNECTION_H__

#if defined(USE_ORACLE)

#include "dbi/DatabaseConnection.h"
#include <oci.h>

/*!
 * \file OracleConnection.h
 * \brief Oracle interface for database connections
 *
 */
namespace dbi
{
/*!
 *  \class OracleResultSet
 *  \brief This class defines the Oracle interface for a result set
 */
class OracleResultSet : public ResultSet
{
public:
    /*!
    *  Default Constructor
    */
    OracleResultSet(OCIStmt * sqlHandle, OCIError* errorHandle, unsigned int rowCount) : ResultSet()
    {
        mSQLHandle = sqlHandle;
        mErrorHandle = errorHandle;
        mRowIndex = 0;
        mRowCount = rowCount;
    }

    /*!
     *  Default Destructor
     */
    ~OracleResultSet()
    {
        OCIHandleFree(mSQLHandle, OCI_HTYPE_STMT);
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
    unsigned int getNumRows();

private:
    OCIStmt* mSQLHandle;
    OCIError* mErrorHandle;
    unsigned int mRowIndex;
    unsigned int mRowCount;
};

/*!
 * \class OracleConnection
 * \brief Oracle database interface
 *
 * This class provides the basis for Oracle connections
 * 
 */
class OracleConnection : public DatabaseConnection
{
public:

    /*!
     *  Default Constructor
     */
    OracleConnection();

    /*!
     * Destructor
     *
     */
    ~OracleConnection();

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
                 unsigned int port = 1521);

    /*!
     *  Disconnect connection to the database
     */
    void disconnect();

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
    const std::string getLastErrorMessage();

private:
    /*! Handles to the Oracle database connection */
    OCIEnv* mEnvHandle;
    OCIError* mErrorHandle;
    OCISvcCtx* mContextHandle;
};
}
#endif
#endif
