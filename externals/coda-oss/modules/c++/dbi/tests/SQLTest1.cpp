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


#include <import/io.h>
#include <import/except.h>
#include <import/sys.h>
#include <import/dbi.h>

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 4) throw except::Exception(Ctxt(FmtX("Usage %s <database> <user> <password> [host] [port]", argv[0])));

        std::string database(argv[1]);
        std::string user(argv[2]);
        std::string password(argv[3]);
        int port = 0;
        std::string host = "";
        if (argc > 5)
        {
            port = (atoi(argv[5]));
        }
        if (argc > 4)
        {
            host = (argv[4]);
        }

        std::string fieldname;

        dbi::pResultSet resultSet;
        dbi::Row myRow;

        dbi::DatabaseClientFactory f;
        dbi::DatabaseConnection * myConn = f.create(database, user, password, host, port);

        //myConn->query("DROP TABLE MyTable");
        std::cout << "HELLO" << std::endl;

        resultSet = myConn->query("select phone from portal_users");

        std::cout << "DID QUERY" << std::endl;

        std::cout << "NUM ROWS " << (int)resultSet->getNumRows() << std::endl;

        std::cout << "ERROR MSG " << myConn->getLastErrorMessage() << std::endl;

        for (unsigned int i = 0; i < resultSet->getNumRows(); i++)
        {
            myRow = resultSet->fetchRow();
            std::cout << "Row " << i << std::endl;
            for (int j = 0; j < myRow.getNumFields(); j++)
            {
                std::cout << "\tField " << j << " name is '" << myRow.getFieldName(j) << "'" << std::endl;
                std::cout << "\tField " << j << " size is '" << myRow.getFieldSize(myRow.getFieldName(j)) <<  "'" << std::endl;
                std::cout << "\tField " << j << " type is '" << myRow.getFieldType(myRow.getFieldName(j)) <<  "'" << std::endl;
                std::string name = myRow.getFieldName(j);
                std::string disp = myRow[name].getData<std::string>(); 
                std::cout << "\tField " << j << " data is '" << disp <<  "'" << std::endl;
            }
        }

        f.destroy(myConn);
    }
    catch (const except::Throwable& t)
    {
        std::cout << "EXCEPTION: " << t.getTrace() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "EXECEPTION: " << e.what() << std::endl;
    }
    return 0;
}
