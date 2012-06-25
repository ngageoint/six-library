#!/usr/bin/env python -O 

"""
Tool used to update the version, project-wide
Add any replacement regexes here to files that include the version
"""

import os, sys, re, fileinput, datetime
from os.path import isdir, join


if __name__ == '__main__':
    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog [options] [path(s)]")
    (options, args) = parser.parse_args()
    
    def hasCopyright(name):
        try:
            f = open(name)
            data = f.read()
            f.close()
            return len(re.findall(r' [*] \(C\) Copyright \d+ [-] \d+', data)) > 0
        except:
            return False
    
    files = []
    paths = args[:]
    while paths:
        path = paths.pop(0)
        if isdir(path):
            paths.extend(map(lambda x: join(path, x), os.listdir(path)))
        elif hasCopyright(path):
            files.append(path)

    print 'Updating copyright in files:\n%s' % ('\n'.join(files))

    year = datetime.datetime.now().year
    for line in fileinput.input(files, inplace=1):
        line = re.sub(r'( [*] \(C\) Copyright \d+ [-] )\d+', r'\g<1>%d' % year, line)
        sys.stdout.write(line)
