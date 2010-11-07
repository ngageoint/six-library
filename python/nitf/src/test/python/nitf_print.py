#!/usr/bin/env python -O 

"""
 * =========================================================================
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
 *
"""

from nitf import *
import os, sys, logging, glob

logging.basicConfig(level=logging.INFO, format='%(message)s', stream=sys.stdout)


def printTRE(tre):
    for fieldname, field in tre:
        logging.info("%s(%s) = '%s'" % (fieldname,
                                        field.getLength(), str(field)))

def print_header(header, desc, extensions={}):
    ''' Dump a header, with possible extensions '''
    logging.info('--- %s ---' % desc)
    logging.info(str(header))
    for section, tres in extensions.iteritems():
        for tre in tres:
            logging.info('--- %s %s TRE [%s] - (%d) ---' % (desc, section, tre.getTag(), tre.getCurrentSize()))
            printTRE(tre)


def print_record(fileName):
    """
    This dumps the file record to standard out
    Notice how we can easily iterate over the Extensions and TREs
    """
    
    handle = IOHandle(fileName)
    record = Reader().read(handle)
    
    logging.info('--- Dumping file: %s ---' % fileName)
    print_header(record.header, "FileHeader",
                 {'UDHD':record.header.getUDHD(),
                  'XHD':record.header.getXHD()})

    for i, segment in enumerate(record.getImages()):
        subheader = segment.subheader
        print_header(subheader, 'Image [%s]' % i,
                     {'UDHD':subheader.getUDHD(),
                      'XHD':subheader.getXHD()})
    for i, segment in enumerate(record.getGraphics()):
        subheader = segment.subheader
        print_header(subheader, 'Graphic [%s]' % i,
                     {'XHD':subheader.getXHD()})
    for i, segment in enumerate(record.getTexts()):
        subheader = segment.subheader
        print_header(subheader, 'Text [%s]' % i, {'XHD':subheader.getXHD()})
       
    handle.close()


if __name__ == '__main__':
    for arg in sys.argv[1:]:
        if os.path.isfile(arg):
            print_record(arg)
        elif os.path.isdir(arg):
            map(print_record, glob.glob(os.path.join(arg, '*.ntf')) + glob.glob(os.path.join(arg, '*.NTF')))
    
