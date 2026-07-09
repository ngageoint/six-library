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

#ifndef __DBI_DATABASE_CLIENT_FACTORY_H__
#define __DBI_DATABASE_CLIENT_FACTORY_H__

#include "dbi/DatabaseConnection.h"

/*!
 * \file DatabaseClientFactory.h
 * \brief Database-independent client creation object
 *
 */

namespace dbi
{
enum DatabaseType
{
    PGSQL = 0,
    MYSQL = 1,
    ORACLE = 2
};

/*!
 * \class DatabaseClient
 * \brief Database-independent client object
 * 
 */
class DatabaseClientFactory
{
public:

    /*!
     *  Default Constructor
     */
    DatabaseClientFactory();

    /*!
     *  Use specific database type
     *  \param dbType  The database to use (if defined)
     */
    DatabaseClientFactory(DatabaseType dbType): mType(dbType)
    {}

    /*!
     * Destructor
     *
     */
    virtual ~DatabaseClientFactory()
    {}

    /*!
     *  Create a connection to the specified database
     *  \param database  The database name
            *  \param user  The username
            *  \param pass  The user password
     *  \param host  The computer host name where the database is located
     *  \param port  The receiving port on the host
     *  \return A database connection is successful
            *  \throw An Exception if unsucessful
     */
    virtual DatabaseConnection * create(const std::string& database,
                                        const std::string& user = "",
                                        const std::string& pass = "",
                                        const std::string& host = "localhost",
                                        unsigned int port = 0);

    /*!
     *  Destroy a previously created connection
     *  \param connection  The connection to destroy
     */
    virtual void destroy(DatabaseConnection * connection)
    {
        if (connection != nullptr)
        {
            connection->disconnect();
            delete connection;
        }
    }

protected:
    DatabaseType mType;
};
}
#endif
