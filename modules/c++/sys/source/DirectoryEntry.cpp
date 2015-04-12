/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#include "sys/DirectoryEntry.h"

bool operator==(const sys::DirectoryEntry::Iterator& lhs,
                const sys::DirectoryEntry::Iterator& rhs)
{
    const sys::DirectoryEntry* lhsD = lhs.get();
    const sys::DirectoryEntry* rhsD = rhs.get();
    if (!lhsD || !rhsD)
    {
        if (lhsD == rhsD) return true;
        return false;
    }
    else
        return ( (lhsD->getName() == rhsD->getName() ) &&
                 (lhsD->getCurrent() == rhsD->getCurrent())
               );

}
bool operator!=(const sys::DirectoryEntry::Iterator& lhs,
                const sys::DirectoryEntry::Iterator& rhs)
{
    const sys::DirectoryEntry* lhsD = lhs.get();
    const sys::DirectoryEntry* rhsD = rhs.get();
    if (!lhsD || !rhsD)
    {
        if (lhsD != rhsD) return true;
        return false;
    }
    else
        return ( (lhsD->getName() != rhsD->getName() ) ||
                 (lhsD->getCurrent() != rhsD->getCurrent())
               );
}
