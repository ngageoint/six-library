/* =========================================================================
 * This file is part of net-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * net-c++ is free software; you can redistribute it and/or modify
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
 * © 2026 The Stratagem Group, LLC, a CACI, INC. - FEDERAL Company
 */

#include <net/net_config.h>

#ifdef NET_CURL_SUPPORT

#include "TestCase.h"
#include <net/CurlHandle.h>
#include <vector>
#include <string>
#include <algorithm>


namespace
{
static bool headerExists(const std::vector<std::string>& headers, const std::string& header)
{
    return std::find(headers.begin(), headers.end(), header) != headers.end();
}
}

TEST_CASE(testSetHttpHeaders)
{
    net::CurlHandle handle;
    
    // Test with empty headers - should result in empty vector
    std::vector<std::string> emptyHeaders;
    handle.setHttpHeaders(emptyHeaders);
    std::vector<std::string> headers = handle.getHeaders();
    TEST_ASSERT_TRUE(headers.empty());
    
    // Test with single header
    std::vector<std::string> singleHeader{"Content-Type: application/json"};
    handle.setHttpHeaders(singleHeader);
    headers = handle.getHeaders();
    TEST_ASSERT_EQ(headers.size(), static_cast<size_t>(1));
    TEST_ASSERT_TRUE(headerExists(headers, "Content-Type: application/json"));
    
    // Test with multiple headers
    std::vector<std::string> multipleHeaders{
        "Content-Type: application/json",
        "Authorization: Bearer token123",
        "User-Agent: TestClient/1.0"
    };
    handle.setHttpHeaders(multipleHeaders);
    headers = handle.getHeaders();
    TEST_ASSERT_EQ(headers.size(), static_cast<size_t>(3));
    TEST_ASSERT_TRUE(headerExists(headers, "Content-Type: application/json"));
    TEST_ASSERT_TRUE(headerExists(headers, "Authorization: Bearer token123"));
    TEST_ASSERT_TRUE(headerExists(headers, "User-Agent: TestClient/1.0"));
    
    // Test that headers can be replaced
    std::vector<std::string> newHeaders{"X-Custom-Header: value"};
    handle.setHttpHeaders(newHeaders);
    headers = handle.getHeaders();
    TEST_ASSERT_EQ(headers.size(), static_cast<size_t>(1));
    TEST_ASSERT_TRUE(headerExists(headers, "X-Custom-Header: value"));
    // Old headers should not exist
    TEST_ASSERT_FALSE(headerExists(headers, "Content-Type: application/json"));
    TEST_ASSERT_FALSE(headerExists(headers, "Authorization: Bearer token123"));
    
}

// Test that setPutRequest doesn't throw
TEST_CASE(testSetPutRequest)
{
    net::CurlHandle handle;
    handle.setPutRequest();

    TEST_SUCCESS;
}

// Test combining setPutRequest with setHttpHeaders
TEST_CASE(testSetPutRequestWithHeaders)
{
    net::CurlHandle handle;
    std::vector<std::string> headers{
        "Content-Type: text/plain",
        "Content-Length: 0"
    };
    
    handle.setHttpHeaders(headers);
    std::vector<std::string> retrievedHeaders = handle.getHeaders();
    TEST_ASSERT_EQ(retrievedHeaders.size(), 2);
    TEST_ASSERT_TRUE(headerExists(retrievedHeaders, "Content-Type: text/plain"));
    TEST_ASSERT_TRUE(headerExists(retrievedHeaders, "Content-Length: 0"));
    
    handle.setPutRequest();
    
    // Headers should still be set after setPutRequest
    retrievedHeaders = handle.getHeaders();
    TEST_ASSERT_EQ(retrievedHeaders.size(), 2);
}

// Test that multiple handles can be configured independently
TEST_CASE(testMultipleHandlesIndependent)
{
    net::CurlHandle handle1;
    net::CurlHandle handle2;
    
    std::vector<std::string> headers1{"X-Handle: 1"};
    std::vector<std::string> headers2{"X-Handle: 2"};
    
    handle1.setHttpHeaders(headers1);
    handle2.setHttpHeaders(headers2);
    handle1.setPutRequest();
    
    // Verify handle1 headers
    std::vector<std::string> h1Headers = handle1.getHeaders();
    TEST_ASSERT_EQ(h1Headers.size(), 1);
    TEST_ASSERT_TRUE(headerExists(h1Headers, "X-Handle: 1"));
    TEST_ASSERT_FALSE(headerExists(h1Headers, "X-Handle: 2"));
    
    // Verify handle2 headers
    std::vector<std::string> h2Headers = handle2.getHeaders();
    TEST_ASSERT_EQ(h2Headers.size(), 1);
    TEST_ASSERT_TRUE(headerExists(h2Headers, "X-Handle: 2"));
    TEST_ASSERT_FALSE(headerExists(h2Headers, "X-Handle: 1"));
}

// Test that calling setHttpHeaders multiple times properly replaces headers
TEST_CASE(testSetHeadersMultipleTimes)
{
    net::CurlHandle handle;
    
    std::vector<std::string> firstHeaders{"X-First: 1"};
    handle.setHttpHeaders(firstHeaders);
    std::vector<std::string> headers = handle.getHeaders();
    TEST_ASSERT_EQ(headers.size(), 1);
    TEST_ASSERT_TRUE(headerExists(headers, "X-First: 1"));
    
    std::vector<std::string> secondHeaders{"X-Second: 2"};
    handle.setHttpHeaders(secondHeaders);
    headers = handle.getHeaders();
    TEST_ASSERT_EQ(headers.size(), 1);
    TEST_ASSERT_TRUE(headerExists(headers, "X-Second: 2"));
    TEST_ASSERT_FALSE(headerExists(headers, "X-First: 1"));
    
    std::vector<std::string> thirdHeaders{"X-Third: 3", "X-Fourth: 4"};
    handle.setHttpHeaders(thirdHeaders);
    headers = handle.getHeaders();
    TEST_ASSERT_EQ(headers.size(), 2);
    TEST_ASSERT_TRUE(headerExists(headers, "X-Third: 3"));
    TEST_ASSERT_TRUE(headerExists(headers, "X-Fourth: 4"));
    TEST_ASSERT_FALSE(headerExists(headers, "X-Second: 2"));
    TEST_ASSERT_FALSE(headerExists(headers, "X-First: 1"));
}

TEST_MAIN(
    TEST_CHECK(testSetHttpHeaders);
    TEST_CHECK(testSetPutRequest);
    TEST_CHECK(testSetPutRequestWithHeaders);
    TEST_CHECK(testMultipleHandlesIndependent);
    TEST_CHECK(testSetHeadersMultipleTimes);
)
#endif
