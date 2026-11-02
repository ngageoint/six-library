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

#ifndef __NET_CURL_INIT_H__
#define __NET_CURL_INIT_H__

#include <net/net_config.h>

#ifdef NET_CURL_SUPPORT

namespace net
{
/*
 *  \class CurlInit
 *  \brief RAII class for the CURL initialization calls that must be called
 *         at least once prior to any other CURL calls.
 *         NOTE: It seems this is not always required, but is probably good
 *         practice.
 */
struct CurlInit
{
    /*
     *  \func Constructor
     *  \brief Initializes CURL
     */
    CurlInit();

    /*
     *  \func Destructor
     *  \brief Shuts down CURL.
     */
    ~CurlInit();
};
}

#endif
#endif
