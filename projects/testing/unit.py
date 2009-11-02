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
 
import traceback

# ------------------------------------------------------
# U      N      I      T    .    P      Y
# ------------------------------------------------------
# Stripped down unit test framework that RegressionRobot
# is built upon.  Loosely based on unittest.py, but 
# without many of the flexibility features that are 
# unnecessary here.  Additionally, this library contains
# a built-in ResultListener that can actually write
# junit-compatible XML.  This opens it up for usage
# within the many Java reporting tools rather than
# requiring us to actually implement text runners.
#
# rrobot.py does attempt a customized runner, but in
# unit.py, for now, a runner is largely unnecessary.
# 
# The unit.py file is independent of rrobot.py, which
# depends upon it (for now) as the unit framework.  This
# allows unit tests fro python to be developed
# independently of the regression component
#
# The components of the package is as follows:
#  * unit.ResultListener - Abstract callback mechanism
#    for processing test results
#
#  * unit.XMLResultListener - JUnit output processing
#    for test results
#
#  * unit.Test - A fixture for unit testing.  Methods
#    beginning with 'test' are processed and scored as
#    is expected in a unit testing framework.
#
#  * unit.Suite - A bundle of unit.Test cases
# ------------------------------------------------------





class ResultListener:
    """
    A ResultListener can listen for unit test results

    Any result listener must be passed into a unit.Suite,
    which, in conjunction with the fixture (unit.Test) will
    trigger its callbacks

    The class does keep track of failures (AssertionError)
    and errors.

    """


    def __init__(self):
        """ Init book-kepeping """
        self.failures = {}
        self.errors = {}
        self.runs = 0


    def __reset(self):
        """
        Clear book-keeping
        """
        self.failures = {}
        self.errors = {}
        self.runs = 0

    def _suiteStart(self, suite):
        """
        Trigger method.  Does book-keeping
        first, then calls onSuiteStart()
        """
        self.__reset()
        self.onSuiteStart(suite)

    def _suiteStop(self, suite):
        """
        Trigger method.  Does book-keeping
        first, then calls onSuiteStop()
        """
        self.onSuiteStop(suite)

    def _testStart(self, testName, fixture):
        """
        Trigger method.  Does book-keeping
        first, then calls onTestStart()
        """
        self.runs += 1
        self.onTestStart(testName, fixture)

    def _testStop(self, testName, fixture):
        """
        Trigger method.  Does book-keeping
        first, then calls onTestStop()
        """
        self.onTestStop(testName, fixture)

    def _testSuccess(self, testName):
        """
        Trigger method.  Does book-keeping
        first, then calls onSuccess
        """
        self.onSuccess(testName)

    def _testError(self, testName, e, e_tb):
        """
        Trigger method.  Does book-keeping
        first, then calls onError()

        """
        self.errors[testName] = (e, e_tb)
        self.onError(testName)

    def _testFailure(self, testName, e, e_tb):
        """ 
        Trigger method.  Does book-keeping
        first, then calls onFailure()
        """
        self.failures[testName] = (e, e_tb)
        self.onFailure(testName)

    def getFailure(self, testName):
        """ Utility to keep sub-classes out of fields """
        return self.failures[testName]

    def getError(self, testName):
        """ Utility to keep sub-classes out of fields """
        return self.errors[testName]

    def onSuiteStart(self, suite):
        """
        Called by _suiteStart() before a
        test suite is run and after any
        internal book-keeping was done
        """
        pass

    def onSuiteStop(self, suite):
        """
        Called by _suiteStop() after
        a test suite was run, and after any
        internal book-keeping was done.

        Suite is the test.Suite class that
        is done running

        """
        pass

    def onTestStart(self, testName, fixture):
        """
        Called when a test method is about to be run,
        Passed by _testStart() after any internal
        book-keeping is done.

        The fixture is the unit.Test object
        """
        pass

    def onTestStop(self, testName, fixture):
        """
        Called by _testStop when a test is finished,
        after any internal book-keeping occurs

        The fixture is the unit.Test object
        """
        pass

    def onSuccess(self, test):
        """
        Called when a success occurs.  The 
        _testSuccess() method calls this after
        any internal book-keeping occurs
        """
        pass

    def onFailure(self, test):
        """
        Called when a failure occurs.  The
        _testFailure() method calls this after
        any internal book-keeping occurs.
        """
        pass

    def onError(self, test):
        """
        Called when a error occurs.  The
        _testError() method calls this after
        any internal book-keeping occurs.

        """
        pass

    def ok(self):
        return len(self.failures) == 0


class XMLResultListener(ResultListener):
    """
    Write out a JUnit compatible XML output.
    In order to write info properly, this
    class needs to save some information that
    is not in the base including the string name
    of each test method that was run

    """
    def __init__(self, stream):
        """
        Requires a file stream as an argument
        """
        ResultListener.__init__(self)
        self.stream = stream
        self.tests = []

    def onSuiteStart(self, suite):
        """ Clear test cache """
        # Dont have to do anything here
        self.tests = []

    def onSuiteStop(self, suite):
        """
        Happens when we stop, we have to write out
        our suite before we lose the results.
        """
        self.stream.write(
    '<testsuite failures="%d" time="%f" errors="%d" tests="%d" name="%s">' %
                     (len(self.failures), 0.1, len(self.errors), self.runs,
                     suite.name))
        for test in self.tests:
            self.stream.write('<testcase time="%f" name="%s">' % (0.1, test))

            if test in self.failures.keys():
                (e, tb) = self.failures[test]
                name = '%s.%s' % (e.__class__.__module__,
                                  e.__class__.__name__)
                self.stream.write('<failure type="%s" message="%s">%s</failure>' % (name, e, tb))
                self.stream.write('<system-out></system-out>')
            self.stream.write('</testcase>')
        self.stream.write('</testsuite>')

    def onTestStart(self, testName, fixture):
        """ Nothing happens here """
        self.tests.append(testName)

    def onTestStop(self, testName, fixture):
        """ Nothing happens here """
        pass

    def onSuccess(self, testName):
        """ Nothing happens here """
        pass

    def onFailure(self, testName):
        """ Nothing happens here """
        pass

    def onError(self, test):
        """ Nothing happens here """
        pass



class Test:
    """
    Test is the fixture for a unit test.  Functions beginning
    with 'test' are called by the fixture, with setUp() and tearDown()
    called before and after.  A unit.Test fires most of the events
    within subscribed listeners.
    """
    def __init__(self):
        """ Nothing happens here """
        pass

    def setUp(self):
        """ 
        When you write a unit test, you may override this
        method with initialization routines that should be
        called before each individual test
        """
        pass

    def tearDown(self):
        """
        When you write a unit test, you may override this
        method with cleanup routines that should be called
        after each individual test
        """
        pass

    def run(self, listeners):
        """
        Go through this looking for methods that start
        with 'test.'  For any matches, check if they are
        callable routines.  Then fire a 
        ResultListener._testStart with the testName and this
        as arguments for each listener.

        Next, try to call setUp(), the method in question,
        then tearDown().  If we succeeded without an
        exception being thrown, trigger a ResultListener.
        _testSuccess().  If an AssertionError occurred, we
        have a failure.  Otherwise, for a normal exception,
        its an Error.  The latter presumably occurs because
        the test case itself is invalid or the environment is
        bad.  For failures, ResultListener._testFailure() is
        triggered.  For errors, ResultListener._testError()
        is triggered

        """
#        for x in self.__dict__.keys():
#            print 'x=%s' % dir(self.__dict__[x])
#        for x in self.__class__.__dict__.keys():
        for x in dir(self):
            if x.startswith('test'):
                #  If its starts with test
                testMethod = getattr(self, x)
                if callable(testMethod):
                    [result._testStart(x, self) for result in listeners]
                    try:
                        self.setUp()
                        testMethod()
                        self.tearDown()
                        [result._testSuccess(x) for result in listeners]
                    except AssertionError, a:
                        print 'FAILURE', a
                        [result._testFailure(x, a, traceback.format_exc()) for result in listeners]
                         

                    except Exception, e:
                        print 'ERROR', e
                        
                        [result._testError(x, e, traceback.format_exc()) for result in listeners]
                    [result._testStop(x, self) for result in listeners]

class Suite:
    """
    A test suite contains one or more test fixtures (unit.Test).
    Suites require a name, which appears in results.  Tests should
    be passed through the constructor, where possible, though the
    add() method will allow them to be added individually.
    
    A Suite has one main method, run() which runs all of the Test
    methods within the fixture.  All listeners are notified of
    events that occur during processing

    """
    def __init__(self, name, tests=[]):
        """
        Each suite has a name, and one or more tests
        Ideally, the application does this initialization
        upfront.
        """
        self.name = name
        self.tests = tests

    def add(self, test):
        """
        Allow the user to add one test fixture at a time
        """
        self.tests.append(test)

    def run(self, listeners):
        """
        Try to run each test.  The suite directly fires
        ResultListener._suiteStart(self) and 
        ResultListener._suiteStop(self), which in turn
        fire onSuiteStart() and onSuiteStop() internally.
        """
        [result._suiteStart(self) for result in listeners]
        for test in self.tests:
            test.run(listeners)
        [result._suiteStop(self) for result in listeners]
