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

#pragma once
#ifndef CODA_OSS_net_CurlHandle_h_INCLUDED_
#define CODA_OSS_net_CurlHandle_h_INCLUDED_

#include <net/config.h>

#ifdef NET_CURL_SUPPORT
#include <curl/curl.h>
#include <string>

namespace net
{
/*
 *  \class CurlHandle
 *  \brief RAII warpper around CURL pointer
 */
class CurlHandle
{
public:
    /*
     *  \func Constructor
     *  \brief Initializes the underlying CURL pointer.
     */
    CurlHandle();

    /*
     *  \func Destructor
     *  \brief Frees the CURL pointer.
     */
    ~CurlHandle();

    /*
     *  \func setURL
     *  \brief Sets the URL that will be used when perform is called.
     *
     *  \param url The desired url.
     */
    void setURL(const std::string& url);

    /*
     *  \func setWriteBuffer
     *  \brief Sets the buffer that curl will write to when perform is called.
     *         CURL simply stores a pointer to this string so this string
     *         MUST outlive the CURL calls!
     *
     *  \param buffer The buffer for CURL to use.
     */
    void setWriteBuffer(std::string& buffer);

    /*
     *  \func disableHostVerification
     *  \brief This disables SSL verification. This should be avoided when
     *         possible.
     */
    void disableHostVerification();

    /*
     *  \func setClientCert
     *  \brief Sets the client certificate used to access the url.
     *
     *  \param certPathname The full path to the certificate on disk.
     */
    void setClientCert(const std::string& certPathname);

    /*
     *  \func setProxy
     *  \brief Sets a proxy url.
     *
     *  \param url The url to the proxy host.
     */
    void setProxy(const std::string& url);

    /*
     *  \func setProxyPort
     *  \brief Sets the proxy port.
     *
     *  \param port The port number for the proxy host.
     */
    void setProxyPort(size_t port);

    /*
     *  \func perform
     *  \brief Performs the underlying CURL call. Before call this, at a
     *         minimum you must also call setURL and setWriteBuffer.
     */
    void perform();

private:
    CurlHandle(const CurlHandle& );
    CurlHandle& operator=(const CurlHandle& );

    static
    void verify(CURLcode code, const std::string& prefix);

    static
    int writeCallback(char* data,
                         size_t size,
                         size_t nmemb,
                         std::string* writerData);

    static
    size_t writeBetterCallback(char* data,
                               size_t size,
                               size_t nmemb,
                               std::string* writerData);
    CURL* const mHandle;
};
}

#endif
#endif  // CODA_OSS_net_CurlHandle_h_INCLUDED_
