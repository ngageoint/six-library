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


def read_copyright_txt(path):
    data = []
    if not os.path.isfile(path):
        print("Failed due to invalid path: " + str(path))
        sys.exit(2)
    with open(path, 'r') as f:
        data = f.readlines()

    return data


def update_copyright_year(crTxt, year):
    crTxt[4] = (" * (C) Copyright 2004 - " + str(year)
                + ", MDA Information Systems LLC\n")
    return crTxt


def update_copyright_module(crTxt, module):
    crTxt[1] = (" * This file is part of " + str(module) + "\n")
    crTxt[6] = (" * " + str(module)
                + " is free software; you can redistribute it "
                + "and/or modify\n")
    return crTxt


# Fix copyright in file
def fix_file(path, crTxt, year, module):
    if not os.path.isfile(path):
        print("Failed to write as path is not a file: " + str(path))
        return

    with open(path, 'r') as f:
        data = f.readlines()

    if len(crTxt) == 0:
        print("No Copyright text provided, assuming copyright exists in file")
        crTxt = data[0:22]  # Assumes copyright is 22 lines (for c++)

    if not len(year) == 0:
        crTxt = update_copyright_year(crTxt, year)

    if not len(module) == 0:
        crTxt = update_copyright_module(crTxt, module)

    # If there is no copyright in the file
    if not any(char in data[0] for char in ['/', '/*', '//']):
        # Append new copyright text
        data = crTxt + "\n" + data
    # If old copyright already exists in file
    else:
        # Rewrite new copyright text
        data[0:len(crTxt)] = crTxt

    # Write updated data to file
    with open(path, 'w') as f:
        f.writelines(data)

    print("Finished writing file: " + str(path))


# Fix copyright in all files in directory
def fix_dir(path, crTxt, year, module):
    if(not os.path.isdir(path)):
        print("Failed due to invalid path: " + str(path))
        sys.exit(2)

    if len(crTxt) == 0:
        print("No text was changed")
        return

    for file in os.listdir(path):
        fix_file(os.path.join(path, file), crTxt, year, module)

    print("Finished writing directory: " + str(path))


# Argument Parsing
if __name__ == '__main__':
    fullCmdArguments = sys.argv

    argList = fullCmdArguments[1:]

    unixOptions = "hc:y:m:d:f:"
    gnuOptions = ["help", "cr", "year", "module", "dir", "file"]

    copyrightTxt = []
    year = ""
    module = ""
    try:
        arguments, values = getopt.getopt(argList, unixOptions, gnuOptions)
    except getopt.error as err:
        print(str(err))
        sys.exit(2)

    for currentArgument, currentVal in arguments:
        if currentArgument in ("-h", "--help"):
            print("[-c, --cr] (Optional) File with new copyright text. \n"
                  + "    If no copyright file is provided, script assumes\n"
                  + "    copyright is first 22 lines of file (for C++) \n"
                  + "[-y, --year] (Optional) Update year in the copyright\n"
                  + "[-m, --module] (Optional) Update module name in the"
                  + " copyright\n"
                  + "[-f, --file] (Conditional) File to update copyright\n"
                  + "[-d, --dir] (Conditional) Directory to update copyright")
        elif currentArgument in ("-c", "--cr"):
            print("Reading new copyright text from file: " + str(currentVal))
            copyrightTxt = read_copyright_txt(currentVal)
        elif currentArgument in ("-y", "--year"):
            year = currentVal
        elif currentArgument in ("-m", "--module"):
            module = currentVal
        elif currentArgument in ("-d", "--dir"):
            fix_dir(currentVal, copyrightTxt, year, module)
        elif currentArgument in ("-f", "--file"):
            fix_file(currentVal, copyrightTxt, year, module)
        else:
            print("Incorrect arguments provided")
            sys.exit(2)
