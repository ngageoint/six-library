import os

'''dumps the libs connected to the targets'''

# Returns either shared or static libs depending on the
# waf configuration
def resolveLibType(env):
    if env.LIB_TYPE == 'stlib':
        return env.STLIB
    else:
        return env.LIB

def dumpLibImpl(context, raw):

    # Recurse through all the wscripts to find all available tasks
    context.restore()
    if not context.all_envs:
        context.load_envs()

    context.recurse([context.run_dir])

    targets = context.targets.split(',')
    libs = []

    for target in targets:
        # Find the target
        tsk = context.get_tgen_by_name(target)

        # Actually create the task object
        tsk.post()

        # Now we can grab his libs
        libs += resolveLibType(tsk.env)
            
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
    context.module(**modArgs)

    context.recurse([context.run_dir])
    
    libs = []
    tsk = context.get_tgen_by_name('dumplib-c++')
    tsk.post()
    libs += resolveLibType(tsk.env)
    
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
            if not raw:
                ret += tsk.env.STLIB_ST % lib
            else:
                ret += lib
        print ret