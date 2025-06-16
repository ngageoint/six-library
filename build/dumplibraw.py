from waflib.Build import BuildContext
from dumplibimpl import dumpLibImpl

class dumplibraw(BuildContext):
    '''dumps the libs connected to the targets'''
    cmd = 'dumplib-raw'
    fun = 'build'

    def execute(self):
        dumpLibImpl(self, True)