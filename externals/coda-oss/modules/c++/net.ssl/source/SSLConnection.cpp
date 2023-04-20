/* =========================================================================
 * This file is part of net.ssl-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * net.ssl-c++ is free software; you can redistribute it and/or modify
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

#include <net/ssl/SSLConnection.h>
#include <net/ssl/SSLExceptions.h>
#if defined(USE_OPENSSL)
 
net::ssl::SSLConnection::SSLConnection(std::unique_ptr<net::Socket>&& socket,
                                       SSL_CTX* ctx,
                                       bool serverAuth,
                                       const std::string& host) :
    NetConnection(std::move(socket)), mServerAuthentication(serverAuth)
{
    mSSL = NULL;

    mBioErr = BIO_new_fp(stderr, BIO_NOCLOSE);

    mSSL = SSL_new(ctx);
    if (mSSL == NULL)
    {
        throw net::ssl::SSLException(Ctxt(FmtX("SSL_new failed")));
    }

    setupSocket(host);
}

net::ssl::SSLConnection::~SSLConnection()
{
    if(mSSL != NULL)
    {
        SSL_shutdown(mSSL);
    }
    if(mSSL != NULL)
    {
        SSL_free(mSSL);
    }
}

void net::ssl::SSLConnection::setupSocket(const std::string& hostName)
{
    net::Socket_T fd = mSocket->getHandle();
    BIO *sbio = BIO_new_socket(fd, BIO_NOCLOSE);
    SSL_set_bio(mSSL, sbio, sbio);
    int val = SSL_connect(mSSL);
    if(val <= 0)
    {
#if defined(__DEBUG_SOCKET)
        if(SSL_get_error(mSSL, val) == SSL_ERROR_WANT_CONNECT)
            printf("ERROR_WANT_CONNECT\n");
        else if(SSL_get_error(mSSL, val) == SSL_ERROR_ZERO_RETURN)
            printf("ERROR_ZERO_RETURN\n");
        else if(SSL_get_error(mSSL, val) == SSL_ERROR_SSL)
        {
            printf("ERROR_SSL: ");
            char buffer[120];
            ERR_error_string(val, buffer);
            BIO_printf(mBioErr, "%s\n", buffer);
        }
        else if(SSL_get_error(mSSL, val) == SSL_ERROR_SYSCALL)
        {
            printf("ERROR_SYSCALL: ");
            char buffer[120];
            ERR_error_string(val, buffer);
            BIO_printf(mBioErr, "%s\n", buffer);
        }
        else if(SSL_get_error(mSSL, val) == SSL_ERROR_NONE)
            printf("NO ERROR: WHY AM I HERE?\n");
#endif

        throw net::ssl::SSLException
            (Ctxt(FmtX("SSL_connect failed: %d", SSL_get_error(mSSL, val))));
    }
    
    if(mServerAuthentication)
    {
        verifyCertificate(hostName);
    }
}

void net::ssl::SSLConnection::verifyCertificate(const std::string& hostName)
{
    // Check that the common name matches the host name
    X509 *peer;
    char peer_CN[256];
    
    /*if(SSL_get_verify_result(mSSL) != X509_V_OK)
      throw net::ssl::SSLException(Ctxt("Certificate doesn't verify"));*/
    
    /* Check the cert chain. The chain length
       is automatically checked by OpenSSL when
       we set the verify depth in the mCtx */
    
    // Check the common name
    peer = SSL_get_peer_certificate(mSSL);
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer),
                              NID_commonName, peer_CN, 
                              256);
    
    if(strcasecmp(peer_CN, hostName.c_str()))
    {
        throw net::ssl::SSLException(Ctxt("Common name doesn't match host name"));
    }
}


sys::SSize_T net::ssl::SSLConnection::read(sys::byte* b, sys::Size_T len)
{
    sys::SSize_T numBytes(0);
    int val(0);
    if (len == 0) return -1;
    
    numBytes = SSL_read(mSSL, (char*)b, len);
    
#if defined(__DEBUG_SOCKET)	
    std::cout << "======= READ FROM SECURE CONNECTION =========" << std::endl;
#endif
    
    if (((val = SSL_get_error(mSSL, numBytes)) != SSL_ERROR_NONE) || 
                        (numBytes == -1 && (NATIVE_SOCKET_GETLASTERROR() != 
                        NATIVE_SOCKET_ERROR(WOULDBLOCK))))
    {
#if defined(__DEBUG_SOCKET)
        std::cout << " Error on read!!!" << std::endl;
        std::cout << "=============================================" << std::endl << std::endl;
#endif

        throw net::ssl::SSLException(Ctxt(FmtX("When receiving %d bytes",
                                               len)) );
    }
    else if (numBytes == 0) 
    {
#if defined(__DEBUG_SOCKET)
        std::cout << " Zero byte read (End of connection)" << std::endl;
        std::cout << "=============================================" << std::endl << std::endl;
#endif
        return -1;
    }
#if defined(__DEBUG_SOCKET)
    std::cout << FmtX("Read %d bytes from socket:", numBytes) << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << std::string(b, numBytes) << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "=============================================" << std::endl << std::endl;
#endif

    return numBytes;
}

void net::ssl::SSLConnection::write(const sys::byte* b, sys::Size_T len)
{
    if (len <= 0) return;
    
    const auto numBytes = SSL_write(mSSL, (const char*)b, len);    
    if (static_cast<sys::Size_T>(numBytes) != len)
    {
        throw net::ssl::SSLException(Ctxt(FmtX("Tried sending %d bytes, %d sent",
                                               len, numBytes)) );
    }
    
#if defined(__DEBUG_SOCKET)	
    std::cout << "========== WROTE TO SECURE CONNECTION =============" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << std::string(b, len) << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "=============================================" << std::endl << std::endl;
#endif
}

#endif

