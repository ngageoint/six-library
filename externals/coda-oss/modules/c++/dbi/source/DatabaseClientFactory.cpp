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

#include "dbi/DatabaseClientFactory.h"
#include "dbi/MySQLConnection.h"
#include "dbi/PgSQLConnection.h"
#include "dbi/OracleConnection.h"

dbi::DatabaseClientFactory::DatabaseClientFactory()
{
#    if defined(USE_PGSQL)
    mType = dbi::PGSQL;
#    elif defined(USE_MYSQL)
    mType = dbi::MYSQL;
#    elif defined(USE_ORACLE)
    mType = dbi::ORACLE;
#    else
    throw except::Exception(Ctxt("No database is defined"));
#    endif
}

dbi::DatabaseConnection * dbi::DatabaseClientFactory::create(const std::string& database,
        const std::string& user,
        const std::string& pass,
        const std::string& host,
        unsigned int port)
{
    dbi::DatabaseConnection * connection = NULL;
#   if defined(USE_PGSQL)
    if (mType == dbi::PGSQL)
    {
        connection = new dbi::PgSQLConnection();
    }
#   endif

#   if defined(USE_MYSQL)
    if (mType == dbi::MYSQL)
    {
        connection = new dbi::MySQLConnection();
    }
#   endif

#   if defined(USE_ORACLE)
    if (mType == dbi::ORACLE)
    {
        connection = new dbi::OracleConnection();
    }
#   endif

    std::string message("");

    if (connection && connection->connect(database, user, pass, host, port))
    {
        return connection;
    }
    else if (connection)
    {
        message = "Could not connect to database: " + connection->getLastErrorMessage();
        destroy(connection);
    }
    else
    {
        message = "Database is not defined!";
    }

    throw except::Exception(Ctxt(message.c_str()));
}
