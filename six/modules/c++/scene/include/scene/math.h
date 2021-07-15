/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#pragma once
#ifndef SIX_scene_math_h_INCLUDED_
#define SIX_scene_math_h_INCLUDED_

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <math/Constants.h>

namespace scene
{
	//
	// Avoid confusion between degrees and radians
	//
	template<typename T>
	struct radians final
	{
		T value;
	};
	template<typename T>
	inline radians<T> as_radians(T a)
	{
		return radians<T> { a };
	}
	template<typename T, typename TReturn = T>
	inline radians<TReturn> to_radians(radians<T> a)
	{
		return radians<TReturn> { a.value };
	}

	template<typename T>
	struct degrees final
	{
		T value;
	};
	template<typename T>
	inline degrees<T> as_degrees(T a)
	{
		return degrees<T> { a };
	}
	template<typename T, typename TReturn = T>
	inline degrees<TReturn> to_degrees(degrees<T> a)
	{
		return degrees<TReturn> { a.value };
	}

	template<typename T, typename TReturn = T>
	inline radians<TReturn> to_radians(degrees<T> a)
	{
		return radians<TReturn> { a.value * math::Constants::DEGREES_TO_RADIANS };
	}
	template<typename T, typename TReturn = T>
	inline degrees<TReturn> to_degrees(radians<T> a)
	{
		return degrees<TReturn> { a.value * math::Constants::RADIANS_TO_DEGREES };
	}

	template <template<typename> typename TAngle, typename T>
	inline T sin(TAngle<T> a)
	{
		return std::sin(to_radians(a).value);
	}
	template <template<typename> typename TAngle, typename T>
	inline T cos(TAngle<T> a)
	{
		return std::cos(to_radians(a).value);
	}
	template <template<typename> typename TAngle, typename T>
	inline T tan(TAngle<T> a)
	{
		return std::tan(to_radians(a).value);
	}
	inline double sin(double a)
	{
		return std::sin(a);
	}
	inline double cos(double a)
	{
		return std::cos(a);
	}
	inline double tan(double a)
	{
		return std::tan(a);
	}
}

#endif // SIX_scene_math_h_INCLUDED_
