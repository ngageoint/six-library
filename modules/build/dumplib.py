from waflib.Build import BuildContext
import os

class dumplib(BuildContext):
    '''dumps the libs connected to the targets'''
    cmd = 'dumplib'
    fun = 'build'
    
    # Returns either shared or static libs depending on the
    # waf configuration
    def resolveLibType(self, env):
        if env.LIB_TYPE == 'stlib':
            return env.STLIB
        else:
            return env.LIB

    def execute(self):

        # Recurse through all the wscripts to find all available tasks
        self.restore()
        if not self.all_envs:
            self.load_envs()

        self.recurse([self.run_dir])
        
        targets = self.targets.split(',')
        libs = []
        
        for target in targets:
            # Find the target
            tsk = self.get_tgen_by_name(target)

            # Actually create the task object
            tsk.post()

            # Now we can grab his libs
            libs += self.resolveLibType(tsk.env)
                
        # Now run again but add all the targets we found
        # This resolves running with multiple targets
        moduleDeps = ''
        for lib in libs:
            moduleDeps += str(lib).split('-')[0] + ' '

        # Add in the original targets again
        for target in targets:
            moduleDeps += str(target).split('-')[0] + ' '

        # Now run with the new module deps
        modArgs = globals()
        modArgs['NAME'] = 'dumplib'
        modArgs['MODULE_DEPS'] = moduleDeps
        
        # We need a source file here so it doesn't think it is headers only
        topDir = tsk.bld.top_dir
        buildDir = os.path.dirname(os.path.realpath(__file__))
        modArgs['SOURCE_DIR'] = os.path.relpath(buildDir, topDir)
        modArgs['SOURCE_EXT'] = 'pyc'
        self.module(**modArgs)

        self.recurse([self.run_dir])
        
        libs = []
        tsk = self.get_tgen_by_name('dumplib-c++')
        tsk.post()
        libs += self.resolveLibType(tsk.env)
        
        # Uselibs are only need in static build.
        # In shared builds they are added to the LIB value automatically
        if tsk.env.LIB_TYPE == 'stlib':
            for uselib in tsk.uselib:
                if tsk.env['LIB_' + uselib]:
                    libs += tsk.env['LIB_' + uselib]
        
        if len(libs) == 0:
            # If we found nothing print that we found nothing
            # otherwise it looks like the command failed.
            print 'No dependencies.'
        else:
            ret = ''
            for lib in libs:
                if ret:
                    ret += ' '
                ret += tsk.env.STLIB_ST % lib
            print ret
