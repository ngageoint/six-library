from waflib.Build import BuildContext
from waflib import Errors

import glob
import os
import shutil
import subprocess


class makewheel(BuildContext):
    '''builds a wheel for easy installation'''
    cmd='makewheel'
    fun='build'

    def makeWheel(self, target, wheelMap):
        if target not in wheelMap:
            raise Errors.WafError('Wheel target {0} not defined'.format(target))

        setupFile = os.path.join(wheelMap[target], 'setup.py')
        shutil.copyfile(setupFile, 'setup.py')

        self.to_log('Creating wheel\n')
        binDir = self.env['install_bindir']
        command = 'pip wheel . --wheel-dir {0} --no-deps'.format(binDir)
        subprocess.call(command.split())
        os.remove('setup.py')


    def execute(self):
        self.restore()
        if not self.all_envs:
            self.load_envs()

        if not self.env['WHEEL_MAP']:
            raise Errors.WafError('No wheel targets defined')

        if self.targets:
            targets = self.targets.split(',')
        else:
            targets = self.env['WHEEL_MAP'].keys()

        for target in targets:
            self.makeWheel(target, self.env['WHEEL_MAP'])


