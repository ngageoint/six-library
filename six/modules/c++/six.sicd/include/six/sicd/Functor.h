#/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#ifndef __SIX_SICD_FUNCTOR_H__
#define __SIX_SICD_FUNCTOR_H__

#include "six/Init.h"
#include <vector>

namespace six
{
namespace sicd
{
/*!
 *   \class Functor
 *   \brief Represents a windowing function, with set parameters
 *
 *   This class is meant as an interface for classes to represent windowing
 *   functions. The parameters are set in the constructor, and the functor
 *   is called with the window size
 */
class Functor
{
public:
    virtual std::vector<double> operator()(size_t n) const = 0;
};

class Identity : public Functor
{
public:
    virtual std::vector<double> operator()(size_t /*n*/) const
    {
        return std::vector<double>();
    }
};

class RaisedCos : public Functor
{
public:
    RaisedCos(double coef);
    virtual std::vector<double> operator()(size_t n) const;
private:
    double mCoef;
};

double bessi(size_t order, double x);
double bessi0(double x);
double bessi1(double x);
double bessin(size_t order, double x);

class Kaiser : public Functor
{
public:
    Kaiser(double beta);
    virtual std::vector<double> operator()(size_t n) const;
private:
    double mBeta;

};
}
}
#endif
