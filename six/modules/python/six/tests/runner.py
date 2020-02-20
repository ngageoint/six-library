import os
import subprocess
from subprocess import call

import utils

class TestRunner(object):

    def __init__(self, testsDir):
        self.testsDir = testsDir
        self.previousFiles = os.listdir(os.getcwd())

    def clean(self):
        currentFiles = os.listdir(os.getcwd())
        for f in currentFiles:
            if f not in self.previousFiles and not os.path.islink(f):
                os.remove(f)

    def createInvocation(self, pathname):
        raise NotImplementedError

    def run(self, testName, *args):
        print('Running {0}'.format(testName))
        pathname = os.path.join(self.testsDir, testName)
        commandArgs = self.createInvocation(pathname)
        commandArgs.extend(args)
        result = False
        if call(commandArgs) == 0:
            print("{0} succeeded!".format(testName))
            result = True
        else:
            print("{0} failed!".format(testName))
        self.clean()
        return result


class PythonTestRunner(TestRunner):
    def __init__(self, pathname):
        TestRunner.__init__(self, pathname)

    def createInvocation(self, pathname):
        return ['python', pathname]

class CppTestRunner(TestRunner):
    def __init__(self, pathname):
        TestRunner.__init__(self, pathname)

    def createInvocation(self, pathname):
        return [utils.executableName(pathname)]

