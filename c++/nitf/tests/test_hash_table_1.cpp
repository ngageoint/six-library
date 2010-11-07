/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.hpp>
#include <fstream>

/*  For this first test case, the DATA is a simple char*   */
/*  This case should be trivial since a data pointer is    */
/*  always the same size in C                              */

int main(int argc, char **argv)
{
    try
    {
        /*  The hash table configurator  */
        std::ifstream config;

        /*  The search file -- look for this text  */
        std::ifstream search;

        /*  A key buffer  */
        std::string keyBuf("");

        nitf::HashTable hashTable(4);

        /*  We didnt allocate the valueBuf -- its static          */
        /*  As a result, we need to notify our hash table NOT to  */
        /*  destroy it on failure                                 */
        hashTable.setPolicy(NITF_DATA_RETAIN_OWNER);

        /*  Make sure our key value file and search file exist  */
        if (argc != 3)
        {
            throw nitf::
            NITFException(Ctxt
                          (FmtX
                           ("Usage: %s <key[tab]value file> <search-key file>\n",
                            argv[0])));
        }
        /*  Open our config file, this contains the key-value pairs we  */
        /*  will insert into our hash, in the order key[tab]value       */
        config.open(argv[1]);

        if (!config.good())
        {
            /*  If we didnt open, freak out at the user  */
            throw nitf::
            NITFException(Ctxt
                          (FmtX
                           ("Could not find file [%s]\n", argv[1])));
        }

        std::vector < char *>values;
        /*  Scan the configuration file, and read into the buffers  */
        while (config.good())
        {
            /*  A value buffer  */
            values.push_back(new char[512]);

            config >> keyBuf >> values[values.size() - 1];

            if (keyBuf.length() > 0)
            {

                printf("Read Key: %s = %s\n", keyBuf.c_str(),
                       values[values.size() - 1]);

                /*  Now comes the important part -- insert  */
                hashTable.insert(keyBuf, values[values.size() - 1]);
            }

            /*  Be nice -- reset buffers  */
            keyBuf = "";
        }
        /*  Close the configurator  */
        config.close();

        /*  Now, lets be expansive and print the list  */
        hashTable.print();

        /*  Open the search file -- this contains keys to search for  */
        search.open(argv[2]);

        /*  If we couldnt open  */
        if (!search.good())
        {
            /*  Die a sad death  */
            throw nitf::
            NITFException(Ctxt
                          (FmtX
                           ("Could not find file [%s]\n", argv[1])));
        }
        /*  Reset the key again, we need it one more time  */
        keyBuf = "";
        while (search.good())
        {
            search >> keyBuf;

            printf("Searching for key [%s] in hash table\n",
                   keyBuf.c_str());

            if (keyBuf.length() > 0)
            {

                try
                {
                    /*  Find the key/value pair  */
                    nitf::Pair where = hashTable.find(keyBuf);

                    /*  Its there in the hash!!  */

                    printf("\t[%s] Located the value! Value: [%s]\n",
                           keyBuf.c_str(), (char *) where.getData());
                }
                catch (except::NoSuchKeyException & t)
                {
                    printf("\t[%s] Search unsuccessful [%s].  No such key\n",
                           keyBuf.c_str(),
			   t.getMessage().c_str());
                }
            }

            /*  Reset the key buffer  */
            keyBuf = "";

        }
        /*  Close the search file  */
        search.close();

        for (unsigned int i = 0; i < values.size(); i++)
            delete[]values[i];

        return 0;
    }
    catch (except::Throwable & t)
    {
        std::cout << t.getMessage() << std::endl;
    }
}
