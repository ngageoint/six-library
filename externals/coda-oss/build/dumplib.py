from waflib.Build import BuildContext
from dumplibimpl import dumpLibImpl

class dumplib(BuildContext):
    '''dumps the libs connected to the targets'''
    cmd = 'dumplib'
    fun = 'build'

    def execute(self):
        dumpLibImpl(self, False)