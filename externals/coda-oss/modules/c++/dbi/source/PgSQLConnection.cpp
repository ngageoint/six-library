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


#if defined(USE_PGSQL)

#include "dbi/PgSQLConnection.h"
#include <sstream>
#include <import/sys.h>

bool dbi::PgSQLConnection::connect(const std::string& database,
                                   const std::string& user,
                                   const std::string& pass,
                                   const std::string& host,
                                   unsigned int port)
{
    if (port == 0) port = 5432;
    std::stringstream portstream;

    portstream << port;

    // Concatenate all the info together because Postgres likes it that way
    std::string connection = " dbname=" + database;
    if (user.length() > 0)
        connection += " user=" + user;
    if (pass.length() > 0)
        connection += " password=" + pass;
    connection += " host=" + host;
    connection += " port=" + portstream.str();

    mDBHandle = PQconnectdb(connection.c_str());

    ConnStatusType connStatus = PQstatus(mDBHandle);

    if (connStatus != CONNECTION_OK)
    {
        return false;
    }

    return true;
}

dbi::pResultSet dbi::PgSQLConnection::query(const std::string& q)
{
    std::string errorMessage;

    // Execute the query and obtain the result set
    PGresult* res = PQexec(mDBHandle, q.c_str());

    // If the result set is NULL then there is some kind of problem
    if (!res)
    {
        errorMessage = PQerrorMessage(mDBHandle);
        disconnect();
        throw dbi::SQLException(Ctxt(errorMessage));
    }

    // Get the status of the last query and decide what to do
    ExecStatusType resultStatus = PQresultStatus(res);
    ConnStatusType connStatus = PQstatus(mDBHandle);

    if (resultStatus == PGRES_TUPLES_OK)
    {
        return dbi::pResultSet(new PgSQLResultSet(res));
    }
    else if (connStatus != CONNECTION_OK)
    {
        // Note: pgsql returns a PGRES_FATAL_ERROR
        // for pretty much everything that wasn't perfect
        // For example, this error will result from trying
        // to add a table that already exists, or dropping
        // one that does.  As of right now, I do not see
        // such errors as "fatal" enough to cause an exception
        // and therefore just ignore them.

        errorMessage = PQerrorMessage(mDBHandle);
        disconnect();
        throw dbi::SQLException(Ctxt(errorMessage));
    }

    return dbi::pResultSet(new PgSQLResultSet(nullptr));
}

dbi::Row dbi::PgSQLResultSet::fetchRow()
{
    // Find the number of fields this row has
    int numFields = PQnfields(mResults);

    // Create a result set auto pointer and give it to the user
    //std::unique_ptr< Row > row(new Row);
    dbi::Row row;

    for (int i = 0; i < numFields; i++)
    {
        row.addField(PQfname(mResults, i),
                     PQftype(mResults, i),
                     PQfsize(mResults, i),
                     PQgetvalue(mResults, mRowIndex, i));
    }
    mRowIndex++;

    return row;
}

#endif
