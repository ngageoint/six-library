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

#include <import/re.h>
#include "TestCase.h"
#include <map>

TEST_CASE(testCompile)
{
    re::Regex rx;
    // test that an invalid regexp throws an exception
    TEST_THROWS(rx.compile("^("));

    // test that a valid regexp compiles
    try
    {
        rx.compile("^(foo)");
    }
    catch (...)
    {
        TEST_FAIL("Compiling a valid regexp should not have thrown exception!");
    }
}

TEST_CASE(testMatches)
{
    re::RegexMatch matches;

    re::Regex rx("abc");
    TEST_ASSERT_FALSE(rx.match("def", matches));
    TEST_ASSERT(matches.empty());

    re::Regex rx2("^([^:]+):[ ]*([^\r\n]+)\r\n(.*)");
    TEST_ASSERT(rx2.match("Proxy-Connection: Keep-Alive\r\n", matches));
    TEST_ASSERT_EQ(matches.size(), 4);

    TEST_ASSERT_EQ(matches[0], "Proxy-Connection: Keep-Alive\r\n");
    TEST_ASSERT_EQ(matches[1], "Proxy-Connection");
    TEST_ASSERT_EQ(matches[2], "Keep-Alive");
    TEST_ASSERT_EQ(matches[3], "");
}

TEST_CASE(testMatchOptional)
{
    re::RegexMatch matches;
    re::Regex rx("([A-Za-z]+)://([^/?#:]+)(?::(\\d+))?(/[^?#:]+)?(?:[?]([^&#/]+(?:[&;][^&;#/]+)*)?)?(?:[#](.*))?");
    std::string url = "http://localhost:80/something/page.com?param1=foo&param2=bar#fragment";

    rx.match(url, matches);
    TEST_ASSERT_EQ(matches.size(), 7);
    TEST_ASSERT_EQ(matches[0], url)
    TEST_ASSERT_EQ(matches[1], "http");
    TEST_ASSERT_EQ(matches[2], "localhost");
    TEST_ASSERT_EQ(matches[3], "80");
    TEST_ASSERT_EQ(matches[4], "/something/page.com");
    TEST_ASSERT_EQ(matches[5], "param1=foo&param2=bar");
    TEST_ASSERT_EQ(matches[6], "fragment");

    url = "http://localhost/page.com";
    matches.clear();
    rx.match(url, matches);
    TEST_ASSERT_GREATER_EQ(matches.size(), 7);
    TEST_ASSERT_EQ(matches[0], url)
    TEST_ASSERT_EQ(matches[1], "http");
    TEST_ASSERT_EQ(matches[2], "localhost");
    TEST_ASSERT_EQ(matches[3], "");
    TEST_ASSERT_EQ(matches[4], "/page.com");
    TEST_ASSERT_EQ(matches[5], "");
    TEST_ASSERT_EQ(matches[6], "");
}

TEST_CASE(testSearch)
{
    re::Regex rx("ju.");
    std::string result = rx.search("arabsdsarbjudarc34ardnjfsdveqvare3arfarg");
    TEST_ASSERT_EQ(result, "jud");
}

TEST_CASE(testSearchAll)
{
    re::RegexMatch matches;
    re::Regex rx("ar");
    rx.searchAll("arabsdsarbjudarc34ardnjfsdveqvare3arfarg", matches);
    TEST_ASSERT_EQ(matches.size(), 7);
    for (size_t ii = 0; ii < matches.size(); ++ii)
    {
        TEST_ASSERT_EQ(matches[ii], "ar");
    }

    matches.clear();
    re::Regex rx2("a[bc]");
    rx2.searchAll("abadabbaccaddaeabaac", matches);
    //            0    1  2       3  4
    TEST_ASSERT_EQ(matches.size(), 5);
    TEST_ASSERT_EQ(matches[0], "ab");
    TEST_ASSERT_EQ(matches[1], "ab");
    TEST_ASSERT_EQ(matches[2], "ac");
    TEST_ASSERT_EQ(matches[3], "ab");
    TEST_ASSERT_EQ(matches[4], "ac");

    // Test the beginning-of-line matching (should only match once)
    matches.clear();
    re::Regex rx3("^bar");
    rx3.searchAll("barbar", matches);
    TEST_ASSERT_EQ(matches.size(), 1);
    TEST_ASSERT_EQ(matches[0], "bar");
}

TEST_CASE(testSearchAllWithOverlap)
{
    re::RegexMatch matches;
    re::Regex rx("[aA]b[aA]");
    rx.searchAll("abAbabAbabAbbAbAaba", matches);
    //            0 1 2 3 4    5  6

    TEST_ASSERT_EQ(matches.size(), 7);
    TEST_ASSERT_EQ(matches[0], "abA");
    TEST_ASSERT_EQ(matches[1], "Aba");
    TEST_ASSERT_EQ(matches[2], "abA");
    TEST_ASSERT_EQ(matches[3], "Aba");
    TEST_ASSERT_EQ(matches[4], "abA");
    TEST_ASSERT_EQ(matches[5], "AbA");
    TEST_ASSERT_EQ(matches[6], "aba");
}

TEST_CASE(testSearchAllJokersWild)
{
    re::RegexMatch matches;
    re::Regex rx1("....");
    rx1.searchAll("0123456789", matches);
    //            0123456

    TEST_ASSERT_EQ(matches.size(), 7);
    TEST_ASSERT_EQ(matches[0], "0123");
    TEST_ASSERT_EQ(matches[1], "1234");
    TEST_ASSERT_EQ(matches[2], "2345");
    TEST_ASSERT_EQ(matches[3], "3456");
    TEST_ASSERT_EQ(matches[4], "4567");
    TEST_ASSERT_EQ(matches[5], "5678");
    TEST_ASSERT_EQ(matches[6], "6789");

    // We want to make sure that when we do jump because the pattern doesn't
    // match that we pick it back up again in the right spot
    matches.clear();
    re::Regex rx2("[xX][xX][xX][xX]");
    rx2.searchAll("__xXXXxxx__XXXXxxXX", matches);
    //              0123     45678

    TEST_ASSERT_EQ(matches.size(), 9);
    TEST_ASSERT_EQ(matches[0], "xXXX");
    TEST_ASSERT_EQ(matches[1], "XXXx");
    TEST_ASSERT_EQ(matches[2], "XXxx");
    TEST_ASSERT_EQ(matches[3], "Xxxx");

    TEST_ASSERT_EQ(matches[4], "XXXX");
    TEST_ASSERT_EQ(matches[5], "XXXx");
    TEST_ASSERT_EQ(matches[6], "XXxx");
    TEST_ASSERT_EQ(matches[7], "XxxX");
    TEST_ASSERT_EQ(matches[8], "xxXX");
}

TEST_CASE(testDotAllFlag)
{
    // This should match "3.3", "3 4", and "4\n2"
    re::RegexMatch matches;
    re::Regex rx1("\\d.\\d");
    rx1.searchAll("3.3 4\n2", matches);
    TEST_ASSERT_EQ(matches.size(), 3);
    TEST_ASSERT_EQ(matches[0], "3.3");
    TEST_ASSERT_EQ(matches[1], "3 4");
    TEST_ASSERT_EQ(matches[2], "4\n2");

    // This should only match the "3.3" if the replaceDot() function
    // is working correctly
    matches.clear();
    re::Regex rx2("\\d\\.\\d");
    rx2.searchAll("3.3 4\n2", matches);
    TEST_ASSERT_EQ(matches.size(), 1);
    TEST_ASSERT_EQ(matches[0], "3.3");
}

TEST_CASE(testMultilineBehavior)
{
    // These tests were put in b/c std::regex treats ^/$ differently
    // in gcc and VS2015, and we want to make sure we've eliminated
    // that difference.

    re::RegexMatch matches;
    re::Regex rx;
    std::string inputString = 
        "3.3 4\n2\nx\r\ns\r\n;sjf sfkgsdkie\n shfihfoisu\nha hosd\nhvfoef\n";

    // This should match just the beginning
    rx.compile("^.");
    rx.searchAll(inputString, matches);
    TEST_ASSERT_EQ(matches.size(), 1);

    // This should match nothing
    matches.clear();
    rx.compile("^.$");
    TEST_ASSERT_FALSE(rx.match(inputString, matches));
    TEST_ASSERT_EQ(matches.size(), 0);

    // This should match the whole inputString
    matches.clear();
    rx.compile("^.*$");
    TEST_ASSERT_TRUE(rx.match(inputString, matches));
    TEST_ASSERT_EQ(matches.size(), 1);
    TEST_ASSERT_EQ(matches[0].length(), inputString.length());

#ifdef RE_ENABLE_STD_REGEX
    // These exercise our limitations and should all throw exceptions (sigh)
    matches.clear();
    TEST_EXCEPTION(rx.compile(".$"));

    matches.clear();
    TEST_EXCEPTION(rx.compile("foo^bar"));

    matches.clear();
    TEST_EXCEPTION(rx.compile("^foo$bar"));
#endif
}

TEST_CASE(testSub)
{
    // Part of the intent here is to make sure we can handle strings
    // substituted that are longer or shorter than what they're
    // replacing
    re::Regex rx("arb");
    std::string subst = rx.sub("Hearbo", "ll");
    TEST_ASSERT_EQ(subst, "Hello");

    subst = rx.sub("Hearbo Kearby!", "ll");
    TEST_ASSERT_EQ(subst, "Hello Kelly!");

    subst = rx.sub("Hearbo Kearby!", "llll");
    TEST_ASSERT_EQ(subst, "Hellllo Kelllly!");

    // So what happens if we're replacing a pattern with something that would
    // also match that pattern itself?
    subst = rx.sub("Hearbo Kearby!", "arbarb");
    TEST_ASSERT_EQ(subst, "Hearbarbo Kearbarby!");

    // And now replace it with nothing at all
    subst = rx.sub("Hearbo Kearby!", "");
    TEST_ASSERT_EQ(subst, "Heo Key!");

    // Make sure we can replace stuff right next to each other
    subst = rx.sub("arbarbarb!", "blah");
    TEST_ASSERT_EQ(subst, "blahblahblah!");

    // Test the matchBeginning flag (internal to sub())
    re::Regex rx2("^bar");
    std::string subst2 = rx2.sub("barbar", "foo");
    TEST_ASSERT_EQ(subst2, "foobar");
}

TEST_CASE(testSplit)
{
    re::Regex rx1("ar");
    std::vector<std::string> vec;
    rx1.split("ONEarTWOarTHREE", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "ONE");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");

    vec.clear();
    rx1.split("ONEarTWOarTHREEar", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "ONE");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");

    re::Regex rx2("x");
    vec.clear();
    rx2.split("ONExTWOxTHREE", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "ONE");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");

    vec.clear();
    rx2.split("ONExTWOxTHREEx", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "ONE");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");

    re::Regex rx3("xxxxxxxxxx");
    vec.clear();
    rx3.split("ONExxxxxxxxxxTWOxxxxxxxxxxTHREE", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "ONE");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");

    vec.clear();
    rx3.split("ONExxxxxxxxxxTWOxxxxxxxxxxTHREExxxxxxxxxx", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "ONE");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");

    // Test the case where match is at the beginning
    vec.clear();
    rx3.split("xxxxxxxxxxTWOxxxxxxxxxxTHREExxxxxxxxxx", vec);
    TEST_ASSERT_EQ(vec.size(), 3);
    TEST_ASSERT_EQ(vec[0], "");
    TEST_ASSERT_EQ(vec[1], "TWO");
    TEST_ASSERT_EQ(vec[2], "THREE");

    // Test the beginning-of-line matching (should only match once)
    vec.clear();
    re::Regex rx4("^bar");
    rx4.split("barfoobar", vec);
    TEST_ASSERT_EQ(vec.size(), 2);
    TEST_ASSERT_EQ(vec[0], "");
    TEST_ASSERT_EQ(vec[1], "foobar");
}

// This was copied out of re/tests/RegexTest3.cpp
TEST_CASE(testHttpResponse)
{
    const char
        *request =
        "GET http://pluto.beseen.com:1113 HTTP/1.0\r\nProxy-Connection: Keep-Alive\r\nUser-Agent: Mozilla/4.75 [en] (X11; U; SunOS 5.6 sun4u)\r\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\nAccept-Encoding: gzip\r\nAccept-Language: en\r\nAccept-Charset: iso-8859-1,*,utf-8\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 96\r\n\r\n";

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

        bool parse(const char* header, size_t length)
        {
            mHeader = std::string(header, length);
            if (!parseRequest())
            {
                if (!parseResponse())
                {
                    return false;
                }
            }

            return true;
        }

        bool parseRest(const std::string& restOfChunk)
        {
            std::string rest = restOfChunk;

            re::RegexMatch matches;
            while (!mMatchEndOfHeader.match(rest, matches))
            {
                re::RegexMatch keyVals;
                if (mMatchPair.match(rest, keyVals))
                {
                    mKeyValuePair[keyVals[1]] = keyVals[2];

                    rest = keyVals[3];
                }
                else
                {
                    return false;
                }
            }

            return true;
        }

        bool parseResponse()
        {
            re::RegexMatch responseVals;
            if (mMatchResponse.match(mHeader, responseVals))
            {
                mVersion = responseVals[1];
                mReturnVal = responseVals[2];

                return parseRest(responseVals[3]);
            }
            else
            {
                return false;
            }
        }

        bool parseRequest()
        {
            re::RegexMatch requestVals;
            if (mMatchRequest.match(mHeader, requestVals))
            {
                mMethod = requestVals[1];
                mUrl = requestVals[2];
                mVersion = requestVals[3];

                return parseRest(requestVals[4]);
            }
            else
            {
                return false;
            }
        }

        std::string getReturnVal() const
        {
            return mReturnVal;
        }
        std::string getUrl() const
        {
            return mUrl;
        }
        std::string getVersion() const
        {
            return mVersion;
        }
        std::string getMethod() const
        {
            return mMethod;
        }

        std::string getContentType() const
        {
            std::string key = "Content-Type";
            return getAssociatedValue(key);
        }
        std::string getContentLength() const
        {
            std::string key = "Content-Length";
            return getAssociatedValue(key);
        }

        std::string getAssociatedValue(const std::string& key) const
        {
            const std::map<std::string, std::string>::const_iterator p =
                    mKeyValuePair.find(key);

            if (p == mKeyValuePair.end())
            {
                return std::string("");
            }
            return p->second;
        }

    protected:
        re::Regex mMatchRequest;
        re::Regex mMatchPair;
        re::Regex mMatchEndOfHeader;
        re::Regex mMatchResponse;
        std::map<std::string, std::string>mKeyValuePair;

        std::string mReturnVal;
        std::string mUrl;
        std::string mVersion;
        std::string mMethod;
        std::string mHeader;
    };

    HttpParser p;
    TEST_ASSERT_TRUE(p.parse(request, strlen(request)));

    TEST_ASSERT_EQ(p.getReturnVal(), "");
    TEST_ASSERT_EQ(p.getMethod(), "GET");
    TEST_ASSERT_EQ(p.getUrl(), "http://pluto.beseen.com:1113");
    TEST_ASSERT_EQ(p.getVersion(), "1.0");
    TEST_ASSERT_EQ(p.getAssociatedValue("User-Agent"), "Mozilla/4.75 [en] (X11; U; SunOS 5.6 sun4u)");
    TEST_ASSERT_EQ(p.getAssociatedValue("Accept-Encoding"), "gzip");
    TEST_ASSERT_EQ(p.getContentType(), "application/x-www-form-urlencoded");
    TEST_ASSERT_EQ(p.getContentLength(), "96");
}

int main(int, char**)
{
    TEST_CHECK(testCompile);
    TEST_CHECK(testMatches);
    TEST_CHECK(testMatchOptional);
    TEST_CHECK(testSearch);
    TEST_CHECK(testSearchAll);
    TEST_CHECK(testSearchAllWithOverlap);
    TEST_CHECK(testSearchAllJokersWild);
    TEST_CHECK(testDotAllFlag);
    TEST_CHECK(testMultilineBehavior);
    TEST_CHECK(testSub);
    TEST_CHECK(testSplit);
    TEST_CHECK(testHttpResponse);
    return 0;
}
