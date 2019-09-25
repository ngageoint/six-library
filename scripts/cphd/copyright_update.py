#!/usr/bin/env python
#
# =========================================================================
# This file is part of cphd-python 
# =========================================================================
# 
# (C) Copyright 2004 - 2019, MDA Information Systems LLC
#
# cphd-python is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public 
# License along with this program; If not, 
# see <http://www.gnu.org/licenses/>.
#

#
# Script to edit copyright dates and company of files
#

import sys
import os
import getopt

def readCopyrightTxt(path):
    data = []
    if not os.path.isfile(path):
        print("Failed due to invalid path: " + str(path))
        sys.exit(2)
    with open(path, 'r') as f:
        data = f.readlines()

    return data

# Fix copyright in file
def fixFile(path, crTxt):
    if not os.path.isfile(path):
        print("Failed to write as path is not a file: " + str(path))
        return

    if len(crTxt) == 0:
        print("No text was changed")
        return

    with open(path, 'r') as f:
        data = f.readlines()

    # If there is no copyright in the file
    if not any(char in data[0] for char in ['/', '/*', '//']):
        # Append new copyright text
        data = crTxt + data

    # If old copyright already exists in file
    else:
        if len(crTxt) > len(data):
            print("Failed due to incorrect number of copyright lines")
            sys.exit(2)

        # Rewrite new copyright text
        data[0:len(crTxt)] = crTxt

    # Write updated data to file
    with open(path, 'w') as f:
        f.writelines(data)

    print("Finished writing file: " + str(path))

# Fix copyright in all files in directory
def fixDir(path, crTxt):
    if(not os.path.isdir(path)):
        print("Failed due to invalid path: " + str(path))
        sys.exit(2)

    if len(crTxt) == 0:
        print("No text was changed")
        return

    for file in os.listdir(path):
        fixFile(os.path.join(path,file), crTxt)

    print("Finished writing directory: " + str(path))


# Argument Parsing
if __name__ == '__main__':
    fullCmdArguments = sys.argv

    argumentList = fullCmdArguments[1:]

    unixOptions = "hc:d:f:"
    gnuOptions = ["help", "cr", "dir", "file"]

    copyrightTxt = []

    try:
        arguments, values = getopt.getopt(argumentList, unixOptions, gnuOptions)
    except getopt.error as err:
        print(str(err))
        sys.exit(2)

    for currentArgument, currentVal in arguments:
        if currentArgument in ("-h", "--help"):
            print("Provide a file with new copyright text a file path with" +
                  "the -f flag or dir path with the -d flag to correct the copyright info")
        elif currentArgument in ("-c", "--cr"):
            print("Reading new copyright text from file: " + str(currentVal))
            copyrightTxt = readCopyrightTxt(currentVal)
        elif currentArgument in ("-d", "--dir"):
            fixDir(currentVal, copyrightTxt)
        elif currentArgument in ("-f", "--file"):
            fixFile(currentVal, copyrightTxt)
        else:
            print("Incorrect arguments provided")
            sys.exit(2)
