#!/usr/bin/env python -O

"""
Tool used to update the version, project-wide
Add any replacement regexes here to files that include the version
"""

import os, sys, re, fileinput

if __name__ == '__main__':

    def helpAndExit():
        print 'usage: %s <major.minor[-suffix]>' % sys.argv[0]
        sys.exit(1)

    if len(sys.argv) <= 1:
        helpAndExit()

    top_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
    fullVersion = sys.argv[1]
    print('Version = %s' % fullVersion)

    #update wscript
    for line in fileinput.input(os.path.join(top_dir, 'wscript'), inplace=1):
        line = re.sub(r'(\s*VERSION\s*=\s*[\']).+([\']\s*)', r'\g<1>%s\g<2>' % fullVersion, line)
        sys.stdout.write(line)

    #update python setup.py
    setupFile = os.path.join(top_dir, 'modules/python/nitf/setup.py')
    for line in fileinput.input(setupFile, inplace=1):
        line = re.sub(r'version\s*=\s*\'.*\',', r"version = '%s'," % fullVersion, line)
        sys.stdout.write(line)

    #update windows installer
    setupFile = os.path.join(top_dir, 'utils/installer/nitro_installer.iss')
    if os.path.isfile(setupFile):
        for line in fileinput.input(setupFile, inplace=1):
            line = re.sub(r'\d+[.]\d+(-\w+)?', fullVersion, line)
#            line = re.sub(r'\d+[.]\d+[-]setup', r'%s-setup' % fullVersion.lower(), line)
#            line = re.sub(r'\d+[.]\d+(?![-]setup)([-]\w+)?', r'%s' % fullVersion, line)
#            line = re.sub(r'\d+[.]\d+.+[-]setup', r'%s-setup' % fullVersion.lower(), line)
#            line = re.sub(r'\d+[.]\d+.+[.]win32[.]exe', r'%s.win32.exe' % fullVersion.lower(), line)
            #line = re.sub(r'\d+[.]\d+((?![-]setup)[-]\w+)?-setup', r'%s-setup' % fullVersion.lower(), line)
            sys.stdout.write(line)


