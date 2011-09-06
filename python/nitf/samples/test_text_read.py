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

logging.basicConfig(level=logging.INFO, stream=sys.stdout,
                    format='%(asctime)s %(levelname)s %(message)s')

def dump_text(fileName):
    handle = IOHandle(fileName)
    reader = Reader()
    record = reader.read(handle)
    logging.info('Dumping file: %s' % fileName)
    
    for i, segment in enumerate(record.getTexts()):
        logging.info('--- Text [%d] ---' % i)
        textReader = reader.newTextReader(i)
        print('Data length = %d\n' % textReader.getSize())
        print('Data: |%s|\n', str(textReader.read(textReader.getSize())))
    handle.close()


if __name__ == '__main__':
    for arg in sys.argv[1:]:
        if os.path.isfile(arg):
            dump_text(arg)
        elif os.path.isdir(arg):
            map(dump_text, glob.glob(os.path.join(arg, '*.ntf')) + glob.glob(os.path.join(arg, '*.NTF')))


