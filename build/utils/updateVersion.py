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
    
    top_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))
    fullVersion = sys.argv[1]
    print 'Version = %s' % fullVersion
    parts = re.match(r'(?P<major>\d+)[.](?P<minor>\d+)([-](?P<suffix>\w+))?',
                     fullVersion).groupdict()
    major, minor, suffix = parts['major'], parts['minor'], parts['suffix']
    
    if not major or not minor:
        helpAndExit()

    if not suffix:
        suffix = ''
    
    #update version of C Makefiles
    makefiles = [os.path.join(top_dir, 'c/nitf/build/Makefile.in'),
                 os.path.join(top_dir, 'java/cgm/src/jni/build/Makefile.in'),
                 os.path.join(top_dir, 'java/nitf/src/jni/build/Makefile.in'),]
    for line in fileinput.input(makefiles, inplace=1):
        line = re.sub(r'(\s*MAJOR_VERSION\s*=\s*)\d+', r'\g<1>%s' % major, line)
        line = re.sub(r'(\s*MINOR_VERSION\s*=\s*)\d+', r'\g<1>%s' % minor, line)
        line = re.sub(r'(\s*VERSION_SUFFIX\s*=\s*).+', r'\g<1>%s' % suffix or '', line)
        sys.stdout.write(line)
    
    #update java properties file
    def replaceInProperties(propsFile):
        for line in fileinput.input(propsFile, inplace=1):
            line = re.sub(r'(\s*version\s*=\s*).+', r'\g<1>%s' % fullVersion, line)
            sys.stdout.write(line)
    
    replaceInProperties(os.path.join(top_dir, 'java/nitf/project.properties'))
    
    #update xml pom files
    def replaceInPom(pomFile):
        found = False #only replace the first one
        for line in fileinput.input(pomFile, inplace=1):
            pattern = r'(<version>\s*).+(\s*</version>)'
            if not found and re.search(pattern, line):
                found = True
                line = re.sub(pattern, r'\g<1>%s\g<2>' % fullVersion, line)
            sys.stdout.write(line)
    
    replaceInPom(os.path.join(top_dir, 'java/nitf/pom.xml'))
    replaceInPom(os.path.join(top_dir, 'java/cgm/pom.xml'))
    
    def replaceInWscript(wscript):
        for line in fileinput.input(wscript, inplace=1):
            line = re.sub(r'(\s*VERSION\s*=\s*[\']).+([\']\s*)', r'\g<1>%s\g<2>' % fullVersion, line)
            sys.stdout.write(line)
    
    replaceInWscript(os.path.join(top_dir, 'c/nitf/wscript'))
    replaceInWscript(os.path.join(top_dir, 'c/cgm/wscript'))
    replaceInWscript(os.path.join(top_dir, 'c++/nitf/wscript'))
    replaceInWscript(os.path.join(top_dir, 'java/nitf/wscript'))
    replaceInWscript(os.path.join(top_dir, 'java/cgm/wscript'))
    
    #update python setup.py
    setupFile = os.path.join(top_dir, 'python/nitf/setup.py')
    for line in fileinput.input(setupFile, inplace=1):
        line = re.sub(r'(\s*version\s*=\s*),.+', r"\g<1>'%s'" % fullVersion, line)
        sys.stdout.write(line)
    
    #update windows installer
    setupFile = os.path.join(top_dir, 'build/installer/nitro_installer.iss')
    for line in fileinput.input(setupFile, inplace=1):
        line = re.sub(r'\d+[.]\d+(-\w+)?', fullVersion, line)
#        line = re.sub(r'\d+[.]\d+[-]setup', r'%s-setup' % fullVersion.lower(), line)
#        line = re.sub(r'\d+[.]\d+(?![-]setup)([-]\w+)?', r'%s' % fullVersion, line)
#        line = re.sub(r'\d+[.]\d+.+[-]setup', r'%s-setup' % fullVersion.lower(), line)
#        line = re.sub(r'\d+[.]\d+.+[.]win32[.]exe', r'%s.win32.exe' % fullVersion.lower(), line)
        #line = re.sub(r'\d+[.]\d+((?![-]setup)[-]\w+)?-setup', r'%s-setup' % fullVersion.lower(), line)
        sys.stdout.write(line)
    
    
