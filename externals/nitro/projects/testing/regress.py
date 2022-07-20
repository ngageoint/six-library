import os
import os.path
import rrobot as rr
import unit

class ImageLoading(rr.RegressionTest):
    def __init__(self, exePath):
        self.exePath = exePath
        self.test_image_loading = '%s/test_image_loading' % (self.exePath)
    
    def test_image_loading_i_3301a(self):
        imageFile = self.library.getResourceName('i_3301a.ntf')
        base = os.path.splitext(os.path.basename(imageFile))[0]
        out = '%s.dmp' % (base)
        rr.Exe.stdOutAsFile([self.test_image_loading, 
                             imageFile], self.sandbox.getResourceName(out))
        self.assertFilesEqual(out)
        for x in os.listdir('.'):
            if x.startswith('i_3301a_ntf_0__1024_1024_8_band_'):
                copy = os.path.basename(x)
                os.rename('./%s' % x, '%s' % self.sandbox.getResourceName(copy))
                self.assertFilesEqual(copy)


class DumpRecord(rr.RegressionTest):
    
    def __init__(self, exePath):
        self.exePath = exePath
        self.test_dump_record = '%s/test_dump_record' % (self.exePath)

    def test_dump_records(self):
        """
        Normally, you would not try and cram all of your tests
        into one unit test, but for demonstration purposes, I
        chose to do it this way here
        """
        for file in self.library.contents():
            if file.endswith('.ntf') or file.endswith('.nsf'):
                print 'Running file ', file
                out = '%s.dmp' % os.path.splitext(os.path.basename(file))[0]
                rr.Exe.stdOutAsFile([self.test_dump_record, self.library.getResourceName(file) ], 
                                    self.sandbox.getResourceName(out))
                self.assertFilesEqual(out)

    def setUp(self):
        rr.RegressionTest.setUp(self)

    def tearDown(self):
        rr.RegressionTest.tearDown(self)


if __name__ == '__main__':
    # Bootstrap master

    import sys
    bootstrapMaster = False
    libraryPath = '/home/dpressel/sample-nitfs'
    exePath = '/home/dpressel/nitro-nitf/trunk/c/nitf/tests/i686-pc-linux-gnu/'
    root = '/home/dpressel/nitro-regression'


    for i in range(len(sys.argv)):
        if sys.argv[i] == '-build':
            bootstrapMaster = True
        elif sys.argv[i] == '-library':
            libraryPath = sys.argv[i+1]
        elif sys.argv[i] == '-root':
            root = sys.argv[i+1]
        elif sys.argv[i] == '-exe':
            exePath = sys.argv[i+1]

    masterPath = root + '/master'
    sandboxPath = root + '/test'

    #  Normally, we would expect this to come from a complete run
    #  of good data, but here we will make it on-the-fly
    if bootstrapMaster is True:
        tdr = '%s/test_dump_record' % exePath
        til = '%s/test_image_loading' % exePath

        if not os.path.exists(sandboxPath):
            os.makedirs(sandboxPath)

        
        masterSuite = masterPath + '/nitro-suite'

        if not os.path.exists(masterSuite):
            os.makedirs(masterSuite)
    
        testRecordPath = '%s/test_dump_records' % (masterSuite)
        if not os.path.exists(testRecordPath):
            print 'Creating directory [%s]', testRecordPath
            os.mkdir(testRecordPath)

        for file in os.listdir(libraryPath):
            nitf = '%s/%s' % (libraryPath, file)
            base = os.path.splitext(os.path.basename(nitf))[0]

            out = '%s/%s.dmp' % (testRecordPath, base)
            rr.Exe.stdOutAsFile([tdr, nitf], out)
            print 'Generated [%s]' % out


        # Try to generate the test_image_loading_3301a() output
        nitf = '%s/%s' % (libraryPath, 'i_3301a.ntf')
        print nitf
        assert( os.path.exists(nitf) )
        testImagePath = '%s/test_image_loading_i_3301a' % (masterSuite)
        print 'TEST: ', testImagePath
        if not os.path.exists(testImagePath):
            print 'Creating directory [%s]', testImagePath
            os.mkdir(testImagePath)
        base = os.path.splitext(os.path.basename(nitf))[0]
        out = '%s/%s.dmp' % (testImagePath, base)
        rr.Exe.stdOutAsFile([til, nitf], out)
        for x in os.listdir('.'):
            if x.startswith('i_3301a_ntf_0__1024_1024_8_band_'):
                print 'Moving "./%s" to "%s/%s"' % (x, testImagePath, x)
                os.rename('./%s' % x, '%s/%s' % (testImagePath, x))

    # Now we are actually going to do some work


    library = rr.FSLibrary(libraryPath)
    master = rr.FSLibrary(masterPath)
    sandbox = rr.FSLibrary(sandboxPath)
    suite = unit.Suite('nitro-suite', 
                       [DumpRecord(exePath), ImageLoading(exePath)])
    runner = rr.RegressionRunner('RR', library, master, sandbox, [suite])
    runner.run()
