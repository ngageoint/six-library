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
#include "six/ProductProcessing.h"

using namespace six;

ProcessingModule* ProcessingModule::clone() const
{
    ProcessingModule* m = new ProcessingModule();
    m->moduleParameters = moduleParameters;

    for (unsigned int i = 0; i < processingModules.size(); ++i)
    {
        ProcessingModule* child = processingModules[i];
        m->processingModules.push_back(child->clone());
    }
    return m;
}

ProcessingModule::~ProcessingModule()
{
    for (unsigned int i = 0; i < processingModules.size(); ++i)
    {
        ProcessingModule* child = processingModules[i];
        delete child;
    }
}

ProductProcessing::~ProductProcessing()
{
    for (unsigned int i = 0; i < processingModules.size(); ++i)
    {
        ProcessingModule* child = processingModules[i];
        delete child;
    }

}

ProductProcessing* ProductProcessing::clone() const
{
    ProductProcessing* p = new ProductProcessing();

    for (unsigned int i = 0; i < processingModules.size(); ++i)
    {
        ProcessingModule* child = processingModules[i];
        p->processingModules.push_back(child->clone());
    }
    return p;
}
