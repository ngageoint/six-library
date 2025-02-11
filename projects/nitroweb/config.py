import web
import os
from os.path import normpath, dirname, join

web.webapi.internalerror = web.debugerror
web.config.baseDir = normpath(dirname(__file__)) + os.sep

web.config.debug = True
web.config.nitfDir = '/data/nitf/files/'
