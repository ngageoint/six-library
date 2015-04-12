/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/DataSource.hpp"

void nitf::DataSource::read(char * buf, nitf::Off size)
        throw (nitf::NITFException)
{
    nitf_DataSource *ds = getNativeOrThrow();
    if (ds && ds->iface)
    {
        if (!ds->iface->read(ds->data, buf, size, &error))
            throw nitf::NITFException(&error);
    }
    else
        throw except::NullPointerReference(Ctxt("DataSource"));
}

nitf::Off nitf::DataSource::getSize()
{
    nitf_DataSource *ds = getNativeOrThrow();
    nitf::Off size = ds->iface->getSize(ds->data, &error);
    if (size < 0)
        throw nitf::NITFException(&error);
    return size;
}

void nitf::DataSource::setSize(nitf::Off size)
{
    nitf_DataSource *ds = getNativeOrThrow();
    if (!ds->iface->setSize(ds->data, size, &error))
        throw nitf::NITFException(&error);
}
