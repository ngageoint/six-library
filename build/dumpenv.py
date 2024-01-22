from waflib.Build import BuildContext

class dumpenv(BuildContext):
    '''dumps the environment'''
    cmd = 'dumpenv'
    fun = 'build'

    def execute(self):
        # Recurse through all the wscripts to find all available tasks
        self.restore()
        if not self.all_envs:
            self.load_envs()
        self.recurse([self.run_dir])

        targets = self.targets.split(',')
        defines = []
        for target in targets:
            # Find the target
            tsk = self.get_tgen_by_name(target)

            # Actually create the task object
            tsk.post()

            # Now we can grab his defines
            defines += tsk.env.DEFINES

        # Sort and uniquify it, then print them all out preceded by the
        # compiler define flag
        defines = sorted(list(set(defines)))

        str = ''
        for define in defines:
            if str:
                str += ' '
            str += self.env['DEFINES_ST'] % define
        print(str)
