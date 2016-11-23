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

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <import/except.h>
#include <import/str.h>
#include <import/re.h>

using namespace str;
using namespace except;
using namespace re;
using namespace std;

const char
        *request =
                "GET http://pluto.beseen.com:1113 HTTP/1.0\r\nProxy-Connection: Keep-Alive\r\nUser-Agent: Mozilla/4.75 [en] (X11; U; SunOS 5.6 sun4u)\r\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\nAccept-Encoding: gzip\r\nAccept-Language: en\r\nAccept-Charset: iso-8859-1,*,utf-8\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 96\r\n\r\n";
//const char *request = "HTTP/1.1 200\r\nContent-Length: 200\r\n\r\n";

class HttpParser
{
public:

    HttpParser()
    {
        mMatchRequest.compile(
                              "^([^ ]+) (http:[^ ]+) HTTP/([0-9]+\\.[0-9]+)\r\n(.*)");
        mMatchPair.compile("^([^:]+):[ ]*([^\r\n]+)\r\n(.*)");
        mMatchEndOfHeader.compile("^\r\n");
        mMatchResponse.compile("^HTTP/([^ ]+) ([^\r\n]+)\r\n(.*)");
    }

    virtual ~HttpParser()
    {
    }

    virtual void parseRest(const std::string& restOfChunk)
    {
        std::string rest = restOfChunk;

        RegexMatch matches;
        while (!mMatchEndOfHeader.match(rest, matches))
        {
            cout << rest << endl;
            RegexMatch keyVals;
            if (mMatchPair.match(rest, keyVals))
            {
                mKeyValuePair[keyVals[1]] = keyVals[2];

                rest = keyVals[3];
            }
            else
            {
                cout << "'rest' doesn't match." << endl;
            }
        }
    }
    virtual bool parseResponse()
    {
        RegexMatch responseVals;
        if (mMatchResponse.match(mHeader, responseVals))
        {
            cout << "'response' matches." << endl;

            mVersion = responseVals[1];
            mReturnVal = responseVals[2];

            parseRest(responseVals[3]);
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual void parse(const char* header, size_t length)
    {
        mHeader = std::string(header, length);
        if (!parseRequest())
            if (!parseResponse())
                assert(0);
    }

    virtual bool parseRequest()
    {
        RegexMatch requestVals;
        if (mMatchRequest.match(mHeader, requestVals))
        {
            cout << "'request' matches." << endl;

            mMethod = requestVals[1];
            mUrl = requestVals[2];
            mVersion = requestVals[3];

            parseRest(requestVals[4]);
            return true;
        }
        else
        {
            return false;
        }
    }

    string getReturnVal()
    {
        return mReturnVal;
    }
    string getUrl()
    {
        return mUrl;
    }
    string getVersion()
    {
        return mVersion;
    }
    string getMethod()
    {
        return mMethod;
    }

    string getContentType()
    {
        string key = "Content-Type";
        return getAssociatedValue(key);
    }
    string getContentLength()
    {
        string key = "Content-Length";
        return getAssociatedValue(key);
    }

    string getAssociatedValue(const string& key)
    {
        map<string, string>::const_iterator p = mKeyValuePair.find(key);
        if (p == mKeyValuePair.end())
        {
            return string("");
        }
        return mKeyValuePair[key];
    }

protected:
    Regex mMatchRequest;
    Regex mMatchPair;
    Regex mMatchEndOfHeader;
    Regex mMatchResponse;
    map<string, string>mKeyValuePair;

    string mReturnVal;
    string mUrl;
    string mVersion;
    string mMethod;
    string mHeader;
};

int main()
{

    try
    {
        HttpParser p;
        p.parse(request, strlen(request));

        cout << "Return Val: " << p.getReturnVal() << endl;
        cout << "Method: " << p.getMethod() << endl;
        cout << "Url: " << p.getUrl() << endl;
        cout << "Version: " << p.getVersion() << endl;
        cout << "User-Agent: " << p.getAssociatedValue("User-Agent") << endl;
        cout << "Accept-Encoding: " << p.getAssociatedValue("Accept-Encoding")
                << endl;
        cout << "Content-Type: " << p.getContentType() << endl;
        cout << "Content-Length: " << p.getContentLength() << endl;
    }
    catch (Throwable& e)
    {
        cout << e.toString() << endl;
    }

    return 0;
}
