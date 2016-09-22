/* =========================================================================
 * This file is part of re-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * re-c++ is free software; you can redistribute it and/or modify
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

#include <import/except.h>
#include <import/str.h>
#include <import/re.h>

using namespace str;
using namespace except;
using namespace re;
using namespace std;

const char* request =
    "GET http://pluto.beseen.com:1113 HTTP/1.0\r\nProxy-Connection: Keep-Alive\r\nUser-Agent: Mozilla/4.75 [en] (X11; U; SunOS 5.6 sun4u)\r\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\nAccept-Encoding: gzip\r\nAccept-Language: en\r\nAccept-Charset: iso-8859-1,*,utf-8\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: 96\r\n\r\n";

int main()
{

    try
    {
        Regex rx1;
        rx1.compile("^([^ ]+) (http:[^ ]+) HTTP/([0-9]+\\.[0-9]+)\r\n(.*)");
        Regex rx2;
        rx2.compile("^([^:]+):[ ]*([^\r\n]+)\r\n(.*)");
        //rx2.compile("^([^:]+):[\s]*([^\r\n]+)\r\n(.*)");//[ ]*([^\r\n])\r\n(.*)");
        Regex rx3;
        rx3.compile("^\r\n");

        std::string escaper("This is 'neato' man?");
        cout << "trying to escape a string" << endl;
        std::string escaped = rx3.escape(escaper);
        cout << "String: " << escaper << " Escaped: " << escaped << endl;
        if (escaper != escaped)
        {
            cout << "It worked " << endl;
        }

        RegexMatch matches;
        if (rx1.match(request, matches))
        {

            cout << "'request' matches." << endl;

            for (size_t i = 1; i < matches.size(); i++)
                cout << "subs[" << i << "] " << matches[i] << endl;

            std::string rest = matches[4];

            RegexMatch matches2;
            while (!rx3.match(rest, matches2))
            {
                cout << rest << endl;
                RegexMatch matches3;
                if (rx2.match(rest, matches3))
                {
                    cout << "'kv' matches." << endl;

                    for (size_t i = 1; i < matches3.size(); i++)
                        cout << "subs[" << i << "] " << matches3[i] << endl;
                    rest = matches3[3];
                }
                else
                {
                    cout << "'rest' doesn't match." << endl;
                    break; // if we get to here, the loop will never end
                }
            }
        }
        else
        {
            cout << "'request' doesn't match." << endl;
        }
    }
    catch (Throwable& e)
    {
        cout << e.toString() << endl;
    }
    return 0;
}
