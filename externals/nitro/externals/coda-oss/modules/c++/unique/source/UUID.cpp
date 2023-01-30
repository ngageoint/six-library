/* =========================================================================
 * This file is part of unique-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * unique-c++ is free software; you can redistribute it and/or modify
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

#include "unique/UUID.hpp"

#ifdef _WIN32
#include <rpc.h>
#else
#include <uuid/uuid.h>
#endif

std::string unique::generateUUID()
{
#ifdef _WIN32
    GUID uuid;
    unsigned char *cResult = NULL;
    
    if ((UuidCreateSequential (&uuid) != RPC_S_OK)
        || (UuidToString(&uuid, &cResult) != RPC_S_OK))
        throw unique::UUIDException("Unable to create UUID");
    std::string sResult((char*)cResult);
    RpcStringFree(&cResult);
    return sResult;
#else
    uuid_t uuid;
    char uuid_buf[37]; // result of uuid_unparse is 36 bytes + '\0'
    uuid_generate(uuid);
    uuid_unparse(uuid, uuid_buf);
    return std::string(uuid_buf);
#endif
}
