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

#include "net/ssl/SSLConnectionClientFactory.h"
#include "net/ssl/SSLExceptions.h"

#if defined(USE_OPENSSL)
namespace net
{
namespace ssl
{
    /*! 
     *  This function is the callback for checking the password instead
     *  of having to enter it in manually for every connection.
     *  It needs the password from the SSLConnectionClientFactory somehow...
     */
    int password_cb(char *buf, int num, int rwflag, void *userdata)
    {
        // Somehow need to obtain a password
        // from an SSLConnectionClientFactory
        /*if(num < (int)strlen("password")+1)
            return 0;
        strcpy(buf, "password");*/
        return strlen("password");
    }
}
}
#endif

void net::ssl::SSLConnectionClientFactory::initializeContext()
{
#if defined(USE_OPENSSL)
    SSL_library_init();
    SSL_load_error_strings();

#if defined(OPENSSL_0_9_8)
    SSL_METHOD *method = SSLv23_client_method();
#else
    const SSL_METHOD *method = SSLv23_client_method();
#endif

    if(method == NULL)
    {
        throw net::ssl::SSLException(Ctxt(FmtX("SSLv23_client_method failed")));
    }
    mCtx = SSL_CTX_new(method);
    if(mCtx == NULL)
    {
        throw net::ssl::SSLException(Ctxt(FmtX("SSL_CTX_new failed")));
    }

    if(mClientAuthentication)
    {
        // Load our keys and certificates
        if(!(SSL_CTX_use_certificate_file(mCtx, mKeyfile.c_str(), SSL_FILETYPE_PEM)))
            throw net::ssl::SSLException(Ctxt("Can't read certificate file"));

        //SSL_CTX_set_default_passwd_cb(mCtx, password_cb);
        if(!(SSL_CTX_use_PrivateKey_file(mCtx, mKeyfile.c_str(), SSL_FILETYPE_PEM)))
            throw net::ssl::SSLException(Ctxt("Can't read key file"));

        // Load the CAs we trust
        if(!(SSL_CTX_load_verify_locations(mCtx, mCAList.c_str(), 0)))
            throw net::ssl::SSLException(Ctxt("Can't read CA list"));

        // Set our cipher list
        if(mCiphers)
        {
            SSL_CTX_set_cipher_list(mCtx, mCiphers);
        }
    }
    
    // Load randomness
    /*
      if(!RAND_load_file("random.pem", 1024*1024))
      throw net::ssl::SSLException(Ctxt("Couldn't load randomness"));
    */
    
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(mCtx, 1);
#endif

#endif
}

net::NetConnection* net::ssl::SSLConnectionClientFactory::newConnection(
        std::unique_ptr<net::Socket>&& toServer)
{
#if defined(USE_OPENSSL)
    return (new SSLConnection(std::move(toServer), mCtx, mServerAuthentication, mUrl.getHost()));
#else
    return (new net::NetConnection(std::move(toServer)));
#endif
}
#if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
net::NetConnection * net::ssl::SSLConnectionClientFactory::newConnection(std::auto_ptr<net::Socket> toServer) 
{
  return newConnection(std::unique_ptr<net::Socket>(toServer.release()));
}
#endif
