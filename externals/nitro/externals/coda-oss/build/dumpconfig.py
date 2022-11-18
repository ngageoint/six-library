from waflib.Build import BuildContext
import subprocess
import os

class dumpconfig(BuildContext):
    '''dumps the libs connected to the targets'''
    cmd = 'dumpconfig'
    fun = 'build'

    def execute(self):
        self.restore()
        if not self.all_envs:
            self.load_envs()

        self.recurse([self.run_dir])

        targetsStr = '--targets=' + self.targets

        # Create the output string
        output = os.path.join(self.env['PREFIX'], 'include')
        output += '\n'
        proc = subprocess.Popen(['python', 'waf', 'dumpenv', targetsStr],
                stdout=subprocess.PIPE,
                universal_newlines=True)
        dump = proc.stdout.read().rstrip().split(' ')

        # Remove PYTHONDIR and PYTHONARCHDIR
        for value in dump:
            if 'PYTHONDIR' not in value and 'PYTHONARCHDIR' not in value:
                output += value + ' '

        output += '\n'
        proc = subprocess.Popen(['python', 'waf', 'dumplib-raw', targetsStr],
                stdout=subprocess.PIPE,
                universal_newlines=True)
        dump = proc.stdout.read().rstrip()
        if not dump.startswith('No dependencies'):
            output += dump
        output += '\n'
        output += os.path.join(self.env['PREFIX'], 'lib')
        output += '\n'

        # Write the output to file
        f = open(os.path.join(self.env['PREFIX'], 'prebuilt_config.txt'), 'w')
        f.write(output)
        f.close()
