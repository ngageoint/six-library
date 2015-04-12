import zipfile, glob, sys
import os, platform, shutil
from subprocess import check_call
from optparse import OptionParser
import platform

parser = OptionParser()
parser.add_option("-p", "--package", dest="package_name", help="Package name")
parser.add_option("-d", "--build-dir", dest="build_dir", help="Build Directory", default=".")
parser.add_option("-c", "--config-options", dest="config_options", help="Configure Options")
parser.add_option("-b", "--build-options", dest="build_options", help="Build Options")
parser.add_option("--studio11-path", dest="studio11_path", help="Sun Studio 11 Compiler Path", default="/var/studio11/SUNWspro")
parser.add_option("--studio12-path", dest="studio12_path", help="Sun Studio 12 Compiler Path", default="/opt/solarisstudio12.3")
parser.add_option("--python27-path", dest="python27_path", help="Python 2.7.x Path", default="/opt/python/v2.7.3")
parser.add_option("--no-distclean", dest="do_distclean", help="No distclean at the end of the build", action="store_false", default=True)

(options, args) = parser.parse_args()

if not options.package_name or not options.build_dir:
    print 'You must specify package name and build directory'
    sys.exit(1)

install_suffix = ''

package_name = options.package_name
build_dir = options.build_dir
if options.config_options is not None:
    config_options = options.config_options.split(',')
else:
    config_options = []
if options.build_options is not None:
    build_options = options.build_options.split(',')
else:
    build_options = []

print 'Package Name: %s' % package_name
print 'Build Dir: %s' % build_dir
print 'Config Options: %s' % config_options
print 'Build Options: %s' % build_options

if 'studio11' in os.environ.get('JOB_NAME'):
    print 'Studio 11 Path: %s' % options.studio11_path
    os.environ['PATH'] += os.pathsep + ('%s/bin' % options.studio11_path)
    os.environ['LD_LIBRARY_PATH'] += os.pathsep + ('%s/lib' % options.studio11_path)
    install_suffix = 'sparc-sun-solaris2.10-64-studio11'
    print 'Path: %s' % os.environ['PATH']
elif 'studio12' in os.environ.get('JOB_NAME') or 'solaris' in os.environ.get('JOB_NAME'):
    print 'Studio 12 Path: %s' % options.studio12_path
    os.environ['PATH'] += os.pathsep + ('%s/bin' % options.studio12_path)
    os.environ['LD_LIBRARY_PATH'] += os.pathsep + ('%s/lib' % options.studio12_path)
    install_suffix = 'sparc-sun-solaris2.10-64-studio12'
elif 'linux' in os.environ.get('JOB_NAME'):
    os.environ['PATH'] += os.pathsep + ('%s/bin' % options.python27_path)
    install_suffix = 'x86_64-linux-gnu-64'
elif 'win32' in os.environ.get('JOB_NAME'):
    install_suffix = 'win32'
    if platform.architecture()[0] == '64bit':
        # We're a 64-bit machine but running a 32-bit job
        java_home32 = os.environ.get('JAVA_HOME_32')
        if java_home32:
            os.environ['JAVA_HOME'] = java_home32
        config_options += ['--enable-32bit']
elif 'win64' in os.environ.get('JOB_NAME'):
    install_suffix = 'win64'
if '-mt' in os.environ.get('JOB_NAME'):
    install_suffix += '-mt'
    config_options += ["--with-crt=MT"]
if '-vc9' in os.environ.get('JOB_NAME'):
	install_suffix += '-vc9'
	config_options += ["--msvc_version=\"msvc 9.0\""]

print 'Job: %s' % os.environ.get('JOB_NAME', '')
print "Revision: %s" % os.environ.get('SVN_REVISION', '')
print "LD_LIBRARY_PATH: %s" % os.environ.get('LD_LIBRARY_PATH','')
install_path = "%s-%s-r%s" % (package_name,install_suffix,os.environ.get('SVN_REVISION', ''))

os.chdir(build_dir)
for f in glob.glob('%s-*' % package_name):
    if os.path.isdir(f):
        shutil.rmtree(f, True)
    else:
        os.remove(f)

check_call(["python", "waf", "distclean"])
check_call(["python", "waf", "configure", "--prefix=%s" % install_path] + config_options)
check_call(["python", "waf", "install"] + build_options)
if options.do_distclean:
    check_call(["python", "waf", "distclean"])

if os.path.isdir(install_path):
    shutil.make_archive(install_path, "zip", install_path)
