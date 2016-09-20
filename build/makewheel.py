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

    def execute(self):
        self.restore()
        if not self.all_envs:
            self.load_envs()

        if not self.env['SETUP_PY_DIR']:
            raise Errors.WafError('Could not make wheel. '
                'Unable to find setup.py')
        shutil.copyfile(os.path.join(self.env['SETUP_PY_DIR'],
            'setup.py'), 'setup.py')
        self.to_log('Creating wheel\n')
        subprocess.call(['pip', 'wheel', '.', '--wheel-dir', self.env['install_bindir'], '--no-deps'])
        os.remove('setup.py')

