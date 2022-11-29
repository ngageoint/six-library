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
 */

#include <net/CurlHandle.h>

#ifdef NET_CURL_SUPPORT
#include <sys/Conf.h>
#include <except/Exception.h>

namespace net
{
CurlHandle::CurlHandle() :
    mHandle(curl_easy_init())
{
    if (mHandle == NULL)
    {
        throw except::Exception(Ctxt("curl_easy_init() failed"));
    }
}

CurlHandle::~CurlHandle()
{
    curl_easy_cleanup(mHandle);
}

void CurlHandle::setURL(const std::string& url)
{
    verify(curl_easy_setopt(mHandle, CURLOPT_URL, url.c_str()),
            "Setting URL");
}

void CurlHandle::setWriteBuffer(std::string& buffer)
{
    verify(curl_easy_setopt(mHandle, CURLOPT_WRITEFUNCTION, writeCallback),
            "Setting write function");

    verify(curl_easy_setopt(mHandle, CURLOPT_WRITEDATA, &buffer),
            "Setting write data");
}

void CurlHandle::disableHostVerification()
{
    verify(curl_easy_setopt(mHandle, CURLOPT_SSL_VERIFYHOST, 0),
            "Disabling host verification");

    verify(curl_easy_setopt(mHandle, CURLOPT_SSL_VERIFYPEER, 0),
            "Disabling host verification");
}

void CurlHandle::setClientCert(const std::string& certPathname)
{
    verify(curl_easy_setopt(mHandle, CURLOPT_SSLCERT, certPathname.c_str()),
            "Setting client cert pathname");
}

void CurlHandle::setProxy(const std::string& url)
{
    verify(curl_easy_setopt(mHandle, CURLOPT_PROXY, url.c_str()),
            "Setting proxy");
}

void CurlHandle::setProxyPort(size_t port)
{
    verify(curl_easy_setopt(mHandle, CURLOPT_PROXYPORT, static_cast<long>(port)),
            "Setting proxy port");
}

void CurlHandle::perform()
{
    verify(curl_easy_perform(mHandle), "curl_easy_perform()");
}

void CurlHandle::verify(CURLcode code, const std::string& prefix)
{
    if (code != CURLE_OK)
    {
        throw except::Exception(Ctxt(prefix + " failed: " +
                curl_easy_strerror(code)));
    }
}

size_t CurlHandle::writeBetterCallback(char* data,
                                       size_t size,
                                       size_t nmemb,
                                       std::string* writeData)
{
    if (writeData == NULL)
    {
        return 0;
    }

    writeData->append(data, size * nmemb);
    return size * nmemb;
}

int CurlHandle::writeCallback(char* data,
                              size_t size,
                              size_t nmemb,
                              std::string* writeData)
{
    return (int) CurlHandle::writeBetterCallback(data, size, nmemb, writeData);
}
}

#endif
