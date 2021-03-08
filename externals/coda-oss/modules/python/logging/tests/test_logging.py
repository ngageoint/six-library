#!/usr/bin/env python

"""
 * =========================================================================
 * This file is part of logging-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2015, MDA Information Systems LLC
 *
 * logging-c++ is free software; you can redistribute it and/or modify
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
 *
"""

import sys
import coda.coda_logging as cl

if __name__ == '__main__':

    ################
    # Logging Test #
    ################

    logger = cl.Logger()
    format = "Thread = %t, Name = %c, Level = %p, File = %F, Method = %M, Line = %L, TimeStamp = %d, Message = %m"
    formatter = cl.StandardFormatter(format)
    handler = cl.StreamHandler()
    handler.setFormatter(formatter)
    logger.addHandler(handler, False)

    logger.debug("Debug Test")
    logger.info("Info Test")
    logger.warn("Warning Test")
    logger.error("Error Test")
    logger.critical("Critical Test")

    cl.warn("Global Warning")
    cl.error("Global Error")
    cl.critical("Global Critical")

    logger.removeHandler(handler)
    logger.warn("This text should not be displayed (No Handler)")
    

    nullHandler = cl.NullHandler()
    logger.addHandler(nullHandler)
    logger.warn("This text should not be displayed (NullHandler)")

    logger.removeHandler(nullHandler)
    fileHandler = cl.FileHandler("FileHandlerTest.txt")
    logger.addHandler(fileHandler)
    logger.warn("This text should be written to file (FileHandler)")

