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


#if defined(USE_MYSQL)

#include "dbi/MySQLConnection.h"
#include <import/sys.h>

bool dbi::MySQLConnection::connect(const std::string& database,
                                   const std::string& user,
                                   const std::string& pass,
                                   const std::string& host,
                                   unsigned int port)
{
    if (port == 0) port = 3306;
    // Attempt to connect to this database and let the user
    // know if it was successful or not
    if (!mysql_real_connect(&mDBHandle, host.c_str(),
                            user.c_str(),  pass.c_str(),
                            database.c_str(), port,
                            nullptr, 0))
    {
        return false;
    }

    return true;
}

dbi::pResultSet dbi::MySQLConnection::query(const std::string& q)
{
    std::string errorMessage;

    MYSQL_RES* results;

    // Perform the specified query and store the result
    if (mysql_real_query(&mDBHandle, q.c_str(), q.size()))
    {
        errorMessage = mysql_error(&mDBHandle);
        disconnect();
        throw dbi::SQLException(Ctxt(errorMessage));
    }

    // If we have no results then this may be okay (insert, create, etc)
    results = mysql_store_result(&mDBHandle);
    if (!results)
    {
        // If the field count is 0 then we know empty results are okay
        // Otherwise there was some kind of error
        if (mysql_field_count(&mDBHandle) != 0)
        {
            errorMessage = mysql_error(&mDBHandle);
            disconnect();
            throw dbi::SQLException(Ctxt(errorMessage));
        }
    }

    // Create a result set auto pointer and give it to the user
    return pResultSet(new MySQLResultSet(results));
}

dbi::Row dbi::MySQLResultSet::fetchRow()
{
    // Holds definition (name, type) for a given field
    MYSQL_FIELD* mysqlField;

    // Fetch the next row in the result set
    MYSQL_ROW mysqlRow = mysql_fetch_row(mResults);

    // Find the number of fields this row has
    int numFields = mysql_num_fields(mResults);

    // Get an array of field lengths for this row
    unsigned long *fieldLengths = mysql_fetch_lengths(mResults);

    // Cannot go any further in giving the correct row
    // and field information, give up
    if (!mysqlRow || !fieldLengths)
    {
        throw SQLException(Ctxt("Cannot retrieve any more rows"));
    }

    // Create a result set auto pointer and give it to the user
    //std::unique_ptr< Row > row(new Row);
    dbi::Row row;

    for (int i = 0; i < numFields; i++)
    {
        // This call retrieves the field definition information
        mysqlField = mysql_fetch_field_direct(mResults, i);

        // Unable to get field definition, something went very wrong
        if (mysqlField)
        {
            row.addField(mysqlField->name,
                         mysqlField->type,
                         fieldLengths[i],
                         mysqlRow[i]);
        }
    }

    return row;
}

#endif
