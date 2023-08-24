import sys, os, re, subprocess
from os.path import isdir, join, split, exists


def cosmoSkyMed(filename):
    path, fname = split(filename)
    if re.match(r'CSK.*.MBI.tif', fname):
        xmlName = join(path, fname.rsplit('MBI.tif')[0] + 'attribs.xml')
        if exists(xmlName):
            return (xmlName, filename)
    return None

def radarsat2(filename):
    path, fname = split(filename)
#    if re.match(r'RS2_.*', split(path)[1]) and re.match(r'imagery_.*.tif', fname):
    if re.match(r'imagery_.*.tif', fname):
        xmlName = join(path, 'product.xml')
        if exists(xmlName):
            return (xmlName, filename)
    return None

def terraSAR(filename):
    path, fname = split(filename)
    if re.match(r'IMAGEDATA', split(path)[1]) and re.match(r'IMAGE_.*.tif', fname):
        parDir, xmlName = split(split(path)[0])
        xmlName = join(parDir, xmlName, '%s.xml' % xmlName)
        if exists(xmlName):
            return (xmlName, filename)
    return None

    
if __name__ == '__main__':
    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog [options] [path(s)]")
    parser.add_option("-d", "--dir", dest='outDir', metavar='DIR',
                      help="specify the output directory (default=cwd)")
    parser.add_option("-x", "--xml", dest='xml', action='store_true',
                      help="Write An XML output")                      
    parser.add_option("-n", "--nitf", dest='nitf', action='store_true',
                      help="Write A NITF output")
    parser.add_option("-t", "--tiff", dest='tiff', action='store_true',
                      help="Write A TIFF output")
    parser.add_option("-k", "--kml", dest='kml', action='store_true',
                      help="Write A KML output")
    parser.add_option("-l", "--level", dest='level', metavar='LEVEL', default=None,
                      help="Level at which to log <debug|info|warn|error>")
    parser.add_option("--limit", dest='limit', metavar='NUM', default=0, type='int',
                      help="Specify a limit on the number of images to process")
    (options, args) = parser.parse_args()
    
    #default to cwd if none provided
    outDir = options.outDir or os.getcwd()
    if not exists(outDir):
        os.makedirs(outDir)

    inputs = []
    paths = args[:]
    while paths:
        path = paths.pop(0)
        if isdir(path):
            paths.extend(map(lambda x: join(path, x), os.listdir(path)))
        else:
            f = cosmoSkyMed(path)
            if not f:
                f = radarsat2(path)
            if not f:
                f = terraSAR(path)
            if f:
                inputs.append(f)
    
    appName = os.path.abspath(join(split(__file__)[0], './install/bin/ScanSARConverter'))
    if 'win32' in sys.platform:
        appName += '.exe'
        
    cmd = [appName]
    cmd.extend(['-d', outDir])
    if options.xml:
        cmd.append('-x')
    if options.nitf:
        cmd.append('-n')
    if options.tiff:
        cmd.append('-t')
    if options.kml:
        cmd.append('-k')
    if options.level:
        cmd.extend(['-l', options.level])
    
    def process(xml, image):
        command = cmd[:]
        command.append('%s,%s' % (xml, image))
        print command
        p = subprocess.Popen(command).communicate()
    
    limit = (options.limit > 0 and min(options.limit, len(inputs)) or len(inputs))
    print 'Processing %d files' % limit
    for i, (xml, image) in enumerate(inputs):
        if i >= limit:
            break
        process(xml, image)
