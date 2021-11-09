/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#ifndef SIX_six_Logger_h_INCLUDED_
#define SIX_six_Logger_h_INCLUDED_
#pragma once

#include <memory>
#include <new> // std::nothrow_t

#include <import/logging.h>

namespace six
{
	// Encapsulate common (and legacy) logging code
	class Logger final
	{
		logging::Logger* pLog_ = nullptr;
		bool ownLog_ = false;

		logging::Logger* &mLog;
		bool &mOwnLog;
		std::unique_ptr<logging::Logger> mLogger;

	public:
		Logger();
		Logger(std::unique_ptr<logging::Logger>&&);
		Logger(logging::Logger&);
		~Logger() = default; // for mOwnLog=true, mLog is managed by std::unique_ptr

		// Legacy class that can't be changed, mLog and mOwnLog are "protected"
		// Don't want to inadvertantly pass non-member data as that will create
		// danging references.
		Logger(logging::Logger*& log, bool& ownLog, std::nullptr_t);

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger(Logger&&) = default;
		Logger& operator=(Logger&&) = delete;

		logging::Logger* get(std::nothrow_t) const;
		logging::Logger& get() const;

        /*!
		 * Set the logger.
		 * \param logger The logger.
		 * \param ownLog Flag for whether the class takes ownership of the
		 *  logger. Default is false.
		 */
        void setLogger(logging::Logger* logger, bool ownLog = false);
		void setLogger(std::unique_ptr<logging::Logger>&&);
        void setLogger(logging::Logger&);
	};
}

#endif // SIX_six_Logger_h_INCLUDED_
