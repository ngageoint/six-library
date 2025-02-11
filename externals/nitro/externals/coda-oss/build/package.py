from waflib.Build import BuildContext

import glob
import os
import shutil
import subprocess
from zipfile import ZipFile


class package(BuildContext):
    '''Creates a zip file of installation dir, and any wheels'''
    cmd='package'
    fun='build'

    def execute(self):
        self.restore()
        if not self.all_envs:
            self.load_envs()

        self.to_log('Zipping installation\n')
        installDir = os.path.dirname(self.env['install_bindir'])

        shutil.make_archive('install', 'zip', None, installDir)

