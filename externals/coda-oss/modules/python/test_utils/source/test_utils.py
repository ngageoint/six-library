import os
import platform
from subprocess import call


def findProjectHome(projectName):
    # Jenkins-friendly environment check
    if 'WORKSPACE' in os.environ:
        return os.environ['WORKSPACE']

    currentPath = os.getcwd()
    while os.path.basename(currentPath) != projectName:
        parent = os.path.abspath(os.path.join(currentPath, os.pardir))

        # If we get to the root directory, something went wrong
        if parent == currentPath:
            raise IOError('Cannot find project home from {}'.format(
                os.getcwd()))
        currentPath = parent

    return currentPath


def installPath(projectName, installDirName):
    home = findProjectHome(projectName)

    installPathname = os.path.join(home, installDirName)
    if os.path.isdir(installPathname):
        return installPathname

    raise IOError('Cannot find install dir {} from {}'.format(
        installDirName, home))


def executableName(pathname):
    if platform.system() == 'Windows':
        if pathname.endswith('.exe'):
            return pathname
        return pathname + '.exe'
    return pathname


def runUnitTests(projectName, installDirName='install'):
    install = installPath(projectName, installDirName)
    unitTestDir = os.path.join(install, 'unittests')
    childDirs = os.listdir(unitTestDir)
    success = True
    for childDir in childDirs:
        for test in os.listdir(os.path.join(unitTestDir, childDir)):
            testPathname = os.path.join(unitTestDir, childDir, test)
            print(testPathname)
            if call([executableName(testPathname)]) != 0:
                success = False

    return success
