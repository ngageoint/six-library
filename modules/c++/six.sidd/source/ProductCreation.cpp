/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/ProductCreation.h"

//using namespace six;
//using namespace six::sidd;

// Windows also has a ProcessorInformation in the API
six::sidd::ProcessorInformation* six::sidd::ProcessorInformation::clone() const
{
    return new six::sidd::ProcessorInformation(*this);
}

six::sidd::ProductCreation* six::sidd::ProductCreation::clone() const
{
    std::auto_ptr<ProductCreation> pc(new six::sidd::ProductCreation());

    if (processorInformation.get())
    {
        pc->processorInformation.reset(processorInformation->clone());
    }
    else
    {
        pc->processorInformation.reset();
    }

    pc->classification = classification;

    ProductCreation* const pcPtr(pc.release());
    return pcPtr;
}
