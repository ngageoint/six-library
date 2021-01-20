/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_COMPLEX_CLASSIFICATION_H__
#define __SIX_COMPLEX_CLASSIFICATION_H__

#include "six/Classification.h"

namespace six
{
namespace sicd
{
/*!
 *  \class ComplexClassification
 *  \brief The implementation of Classification for complex products
 */
class ComplexClassification: public Classification
{
public:
    virtual std::string getLevel() const
    {
        return level;
    }

    virtual std::ostream& put(std::ostream& os) const
    {
        os << "Level: " << level << "\n";
        return os;
    }

    bool operator==(const ComplexClassification& rhs) const
    {
        return (level == rhs.getLevel() &&
            fileOptions == rhs.fileOptions);
    }

    //! This is spelled out (i.e. 'UNCLASSIFIED')
    std::string level;

private:
    virtual bool equalTo(const Classification& rhs) const
    {
        const ComplexClassification* classification = dynamic_cast<const ComplexClassification*>(&rhs);
        if (classification != nullptr)
        {
            return *this == *classification;
        }
        return false;
    }
};
}
}
#endif

