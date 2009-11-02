# =========================================================================
# This file is part of RegressionRobot
# =========================================================================
# 
#
# RegressionRobot is free software; you can redistribute it and/or modify
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
# ------------------------------------------------------------------------
 

# ------------------------------------------------------
# R      R      O      B      O      T    .    P      Y
# ------------------------------------------------------
# Simple python regression testing framework with
# minimal components.  The main things to know
#
# Library:
#   - Data content repository.  Abstraction for some
#   sort of testing backend.  Curent impls are FSLibrary
#   for a file system, and a partial impl for HTTP
#   (HTTPLibrary)
#
# Master Library:
#   - A library of good runs, that new runs should be
#     compared against.  For each unit test method, there
#     must be a directory in the master library.
#     TODO: add utilities to help with this
# 
# Test Sandbox Library:
#   - A library of current runs -- output within the
#     test library will typically match (or come close)
#     to that within the regression libs
#     TODO: there is still to much manual code for comparisons
#     automate any file testing between directories to make
#     this easier
# 
# Data Library:
#   - Library of test input.  This typically contains imagery
#   or data parameters.  No structure is imposed on this library
#   The user makes requests within the test for resources from
#   this library.  Like the master library, the test should not
#   attempt to add data here
#
# TODO: Make it easier to use boilerplate comparison, master 
# migraation, and prevent the user from inadvertantly screwing
# up one of the non-sandbox libraries by using an immutable view
# ------------------------------------------------------
import os
import os.path
import filecmp
import subprocess
import urllib2
import unit
import shutil
import itertools

HTML_ESCAPE = {
    "&": "&amp;",
    '"': "&quot;",
    "'": "&apos;",
    ">": "&gt;",
    "<": "&lt;",
}
   
def escapeHTML(text):
    """ Produce entities within text. """
    lst = []
    for c in text:
        lst.append(HTML_ESCAPE.get(c,c))
    return ''.join(lst)

class Library:
    """
    Think of this as a virtual file system for getting input resources

    """
    def __init__(self):
        pass

    def getResourceName(self, resource):
        """
        Get the local resource corresponding to this
        """
        raise Exception('Someone forgot to implement me')

    def getResource(self, resource):
        """
        Get the local resource corresponding to this
        """
        raise Exception('Someone forgot to implement me')

    def copyResourceToFile(self, resource, file):
        """
        Copy a resource to file specified by a string
        name parameter (file).  Line by line copy
        using generator
        """
        f = open(file, 'w+b')
        for line in self.getResource(resource):
            f.write( line )
        f.close()

    def getResourceSize(self, resource):
        """
        Get the number of bytes in the resource.  Not sure
        that this impl.  belongs here.  Should probably pass
        (sleep-coding) ;)
        """
        return os.path.getsize(self.getResourceName(resource))
        
    def hasResource(self, resource):
        """
        Tell us if a resource exists in the Library.  Not
        sure that this impl. belongs here either.  (More
        sleep-coding)
        """
        return os.path.exists(self.getResourceName(resource))

    def findSubLibrary(self, directory):
        """
        Get a Library object back that corresponds to
        the sub-diretory.  This must exist already
        """
        pass

    def makeSubLibrary(self, directory):
        """
        Create a Library object back that corresponds to
        a new sub-diretory.  This must not exist already
        (we are creating it here)
        """

        pass

    def removeSubLibrary(self, directory):
        """
        Delete a sub-library within a library.
        This is typicall a sub-directory on a file
        system or web server
        """
        pass

    def contents(self):
        pass


class HTTPLibrary(Library):
    """
    This is an HTTP, WebDAV implementation of a library.
    Unfortunately, the WebDAV portion is not implemented
    yet, so many of the functions do not work yet
    """
    def __init__(self, libraryURL):
        self.libraryURL = libraryURL

    def getResourceName(self, resource):
        """
        GET the full path for the relative URL
        """
        return self.libraryURL + '/' + resource

    def getResource(self, resource):
        """
        GET the file given by the relative name
        """
        resourceName = self.getResourceName(resource)
        handle = urllib2.urlopen(resourceName)
        for line in handle:
            yield line
    

    def __httpHead(self, resource):
        """
        Its cheaper to call HEAD HTTP methods, since they
        are required not to give back a content-body.  So
        whenever we need props, try to use this method
        """

        request = urllib2.Request(self.getResourceName(resource))
        request.get_method = lambda: "HEAD"
        response = urllib2.urlopen(request)
        return response

    def getResourceSize(self, resource):
        """
        It would be nice if the HEAD method was guaranteed
        to give back a Content-Length.  Sadly, this doesnt
        appear to be the case, so we return -1 if there 
        wasnt a Content-Legnth Header
        """
        response = self.__httpHead(resource)
        if response.headers.has_key('Content-Length'):
            cl = int(response.headers["Content-Length"])
        else:
            cl = -1
        response.close()
        return cl


    def hasResource(self, resource):
        """
        Try to see if a resource exists using the
        HTTP HEAD method.  More efficient than any
        other way I could think of
        """
        try:
            response = self.__httpHead(resource)
            response.close()
            return True
        except:
            return False


    def findSubLibrary(self, directory):
        """
        Check if the resource exists.  If so, return
        a new HTTP library with the resource

        
        """
        if self.hasResource(directory):
            return HTTPLibrary(self.getResourceName(directory))

    def makeSubLibrary(self, directory):
        """ 
        Until somebody implements basic auth and MKCOL,
        this method is not usable, which means that master
        and runs collections should use FSLibrary instead.

        This isnt really a big deal, since most Libraries
        that use HTTP will be data or master components.
        Its unlikely that users will ever want a test
        sandbox to be on another node.
        """
        pass

    def removeSubLibrary(self, directory):
        """
        Until somebody implements basic auth and MKCOL,
        this method is not usable, which means that master
        and runs collections should use FSLibrary instead.

        This isnt really a big deal, since most Libraries
        that use HTTP will be data or master components.
        Its unlikely that users will ever want a test
        sandbox to be on another node.

        """
        pass

    def contents(self):
        """
        W/O a PROPFIND, this is hard to do, so we pass
        on it for now.  Otherwise, we could just keep
        a list of files in magic file somewhere.

        """

        pass


class FSLibrary(Library):
    """
    This is a file system implementation of a Library

    """
    def __init__(self, root):
        """
        Init, with a file system root
        """
        Library.__init__(self)
        if os.path.samefile(root, '/') or os.path.samefile(root, '/usr/') or os.path.samefile(root, '/bin/'):
            raise Exception('Cannot bind to directory: ', self.root)
        self.root = root

    def getResourceName(self, resource):
        """
        Try to find the file in the local directory
        """
        #if os.path.exists(self.root + '/' + resource) is False:
        #    raise Exception('Failed to find ' + resource + ' in library')
        return self.root + '/' + resource

    def getResource(self, resource):
        """
        Open a file as binary.  The given resource (string)
        is a relative path on the Library
        """
        f = open(self.getResourceName(resource), 'r+b')
        for resource in f:
            yield resource

        f.close()

    def findSubLibrary(self, directory):
        """
        Find a sub-library (here a sub-directory), and return
        it as an FSLibrary.
        """
        if self.hasResource(directory) is False:
            raise Exception('No such directory %s' % directory)
            
        return FSLibrary(self.getResourceName(directory))

    def contents(self):
        """
        Just list the contents of this directory
        """
        return os.listdir(self.root)

    def removeSubLibrary(self, directory):
        """
        Calls shutil.rmtree on the absolute equivalent
        of directory.  This method can shoot a leg off,
        so be careful!
        """
        path = self.getResourceName(directory)

        if not os.path.isdir(path):
            return

        shutil.rmtree(path, ignore_errors=True) 
        

    def makeSubLibrary(self, directory):
        """
        Make a sub-directory, using os.makedirs.
        The new directory is returned as a
        Library object
        """
        path = self.getResourceName(directory)
        os.makedirs(path)
        return FSLibrary(path)

class Exe:

    """
    Ops for playing around with Executables
    """
    @staticmethod
    def stdOutAsLines(_lst):
        """
        Run the executable and yield each line of stdout
        This allows you to do something like this:
        
        for l in rr_exe_out_gen(['ls', '-l'])
        """
        f = subprocess.Popen(_lst, stdout=subprocess.PIPE)
        for item in f.stdout:
            yield item

    @staticmethod
    def stdOutAsString(_lst):
        """
        Run the executable and get back the stdout as a string
        """
        output = subprocess.Popen(_lst, stdout=subprocess.PIPE).communicate()[0]
        return output

    @staticmethod
    def stdOutAsFile(_lst, outname, ops='w+'):
        """
        Run the executable and get back the stdout as a file
        """
        f = subprocess.Popen(_lst, stdout=subprocess.PIPE)
        of = open(outname, ops)
        for item in f.stdout:
            of.write(item)
        of.close()

#
#           Library         Master         Sandbox
# tdr       in.nitf         out.dmp        out.dmp
# til       in.nitf         *band.out      *band.out
# tw3       in.nitf         out.nitf       out.nitf
#


class RegressionSetup(unit.ResultListener):
    """
    This class sets up the directories that are
    necessary for regression test methods.
    
    It should be added as the first listener to
    a regression test suite
    """
    def __init__(self, library, master, sandbox):
        unit.ResultListener.__init__(self)
        self.directoryStack = []
        self.library = library
        self.master = master
        self.sandbox = sandbox

    def onSuiteStart(self, suite):
        """
        When a suite is starting, need to push
        a dieectory with the suite's name on
        the directory stack
        """
        self.directoryStack.append( suite.name )

    def onSuiteStop(self, suite):
        """
        When a suite is over, pop the suite name off
        """
        self.directoryStack.pop()

    def onTestStart(self, testName, fixture):
        """
        When a test is about to start, we need to
        create its directory in the sandbox, add
        the directory to the stack, and init the
        Fixture for the test case.  This is kind
        of intrusive, but it gets the job done.
        """
        self.directoryStack.append( testName )
        directory = '/'.join(self.directoryStack)
        masterSub = self.master.findSubLibrary(directory)
        self.sandbox.removeSubLibrary(directory)
        sandboxSub = self.sandbox.makeSubLibrary(directory)
        fixture.initMaster(masterSub)
        fixture.initSandbox(sandboxSub)
        fixture.initLibrary(self.library)
        
    def onTestStop(self, testName, fixture):
        """
        When a test has ended, we need to pop the
        method name directory off our stack
        """
        self.directoryStack.pop()

class RegressionTest(unit.Test):
    """
    RegressionTest is a type of unit.Test fixture that
    handles regression testing.  It mainly works as follows:
    
    The test has access to a data Library.  This typically
    contains test case input, possibly imagery or parameter
    options.

    The test also has a master Library, which is the Library
    that test outputs are graded on.  Master data should not
    be altered by the test case.  

    The test has a sandbox Library.  This contains any output
    of the program.  This information should be tested against
    the master.
    
    
    """


    def __init__(self):
        """
        Set the master, the data library and the sandbox
        to None
        """
        self.library = None
        self.master = None
        self.sandbox = None
        self.method = None
        

    def initLibrary(self, library):
        """
        Allow RegressionSetup to tell us where the data
        Library is
        """
        self.library = library

    def getLibrary(self):
        """ Get the data Library """
        return self.library

    def initMaster(self, master):
        """
        Allow RegressionSetup to tell us where the master.
        Library is.  
        """

        self.master = master

    def getMaster(self):
        """ Get master Library """
        return self.master

    def initSandbox(self, sandbox):
        """
        Allow RegressionSetup to tell us where the test
        sandbox is.  
        """
        self.sandbox = sandbox

    def getSandbox(self):
        """ Get sandbox test Library """
        return self.sandbox

    def getMasterPath(self, resource):
        """
        Get the resource out of the master library
        """
        return self.master.getResourceName(resource)

    def getSandboxPath(self, resource):
        """
        Get the resource out of the local library
        """
        return self.sandbox.getResourceName(resource)

    def assertFilesEqual(self, resourceName):
        """
        Careful to use abstract methods.  We cant just call
        filecmp.cmp() if we want it to be general

        """
        line = 1
        masterResource = self.master.getResourceName(resourceName)
        testResource = self.sandbox.getResourceName(resourceName)

        print 'Comparing files:\n\t[%s]\n\t[%s]' % (masterResource, testResource)
        
        for masterLine, testLine in itertools.izip(self.master.getResource(resourceName),
                                                   self.sandbox.getResource(resourceName)):
            if masterLine != testLine:
                raise AssertionError('files not equal @ line [%d]:\n\t[%s]\n\t[%s]:\nLHS %s\nRHS %s' %
                                     (line,
                                      masterResource,
                                      testResource,
                                      escapeHTML(masterLine),
                                      escapeHTML(testLine)
                                      )
                                     )
            line += 1
        

class RegressionRunner:
    """
    Simple runner that wires up the loose ends so that regression
    testing is easy to do.  The main pieces of information are
    the base libraries needed for data, master info an testing,
    and the tests themselves.  Additionally, an XMLResultListener
    is wired in automatically, for use with JUnit reporting
    
    Each test suite produces its own XML file
    
    """
    def __init__(self, name, library, master, sandbox, suites=[]):

        self.suites = suites
        self.name = name
        self.master = master
        self.sandbox = sandbox
        self.library = library

    def run(self):
        """
        Run every suite, initializing a results XML file for
        storing test output
        """
        for suite in self.suites:
            xmlFile = open('%s-results.xml' % suite.name, 'w+')
            suite.run([RegressionSetup(self.library,
                                       self.master,
                                       self.sandbox), 
                       unit.XMLResultListener(xmlFile)])

            xmlFile.close()




#if __name__ == '__main__':
#    library = FSLibrary('/home/dpressel/regression-lib/data')
#    master = FSLibrary('/home/dpressel/regression-lib/master')
#    sandbox = FSLibrary('/home/dpressel/regression-lib/tests')
#
#    suite = unit.Suite('regression-suite1', [MyTest()])
#    rr = RegressionRunner('RR', library, master, sandbox, [suite])
#    rr.run()
