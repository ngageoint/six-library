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
        if (argc != 6) throw except::Exception(Ctxt(FmtX("Usage %s <database> <user> <password> <host> <port>", argv[0])));

        std::string database(argv[1]);
        std::string host(argv[4]);
        int port(atoi(argv[5]));
        std::string user(argv[2]);
        std::string password(argv[3]);

        std::string fieldname;

        dbi::pResultSet resultSet;
        dbi::Row myRow;

        dbi::DatabaseClientFactory f;
        dbi::DatabaseConnection * myConn = f.create(database, user, password, host, port);

        myConn->query("DROP TABLE MyTable");

        resultSet = myConn->query("CREATE TABLE MyTable (id INT NOT NULL PRIMARY KEY, name VARCHAR(25) NOT NULL, description TEXT NOT NULL)");

        char document[5];
        document[0] = 'J';
        document[1] = 'E';
        document[2] = '\0';
        document[3] = 'F';
        document[4] = 'F';

        resultSet = myConn->query("SELECT TMODEL_KEY, NAME, OVERVIEW_URL FROM TMODEL");

        myRow = resultSet->fetchRow();
        const char *x = myRow["overview_url"].getData<std::string>().c_str();
        std::string str = myRow["overview_url"].getData<std::string>();
        std::string y = myRow["name"].getData<std::string>();

        std::cout << "\tChar* is '" << x <<  "'" << std::endl;
        std::cout << "\tString is '" << str << "'" << std::endl;
        std::cout << "\tInt is '" << y << "'" << std::endl;
        f.destroy(myConn);
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
    return 0;
}
