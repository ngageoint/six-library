This library defines APIs for accessing multiple kinds of databases
using a common interface.

Currently, MySQL, PostgreSQL, and Oracle are implemented.

To enable this module, from the top source directory run:
$  ./configure --enable-sql-layer=[mysql | pgsql | oracle] --with-sql-home=/path/to/sql --prefix=/path/to/install
$  make
$  make install

There is no special support via the API for binary objects (e.g., you
cannot simply put a Word document into a buffer and then insert it
with a normal query).  Special precautions need to be taken in order
to properly escape certain characters that may invalidate the query
string.

*For MySQL, see mysql_real_escape_string.
*For PostrgeSQL, see lo_import and lo_export for BLOB data.

*MySQL Notes:
 None at this time.  

*PostgreSQL Notes:
 In order to be able to accept hosts during connect time, the 
 database must be configured to have tcpip_socket enabled (which
 is disabled by default).  The conf file is postgresql.conf in 
 the data directory (or wherever your database is).

 Postgres converts basically everything to lower case.  For
 example, if you wanted to do this:

 SELECT field1 AS myField FROM MyTable;

 You would have to access 'myField' as 'myfield':

 (*row)["myfield"];

*Oracle Notes:
 You must have your $ORACLE_HOME environment variable set since oracle needs 
 to be able to find its TNS names.  On that note, only the database name,
 user, and password are pertinent for connecting to an Oracle DB.  The
 localhost and port variables are not used.

 Currently, the only way to get the number of rows from a SELECT query is to
 do a SELECT COUNT(*) query.  This is done automatically in the wrapper for
 all SELECT statements.  The COUNT(*) is simply prepended to the projection
 and then passed to the result set object.
