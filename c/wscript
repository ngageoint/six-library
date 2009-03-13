#!/usr/bin/env python
# encoding: utf-8

import os, sys, glob, types, inspect, tarfile, shutil
sys.path.insert(0, '../build/utils/waf')
import utils as wafUtils
del sys.path[0]

import Task, Constants, Utils, Build, Options

VERSION = '2.0.1-dev'
APPNAME = 'nitro'
srcdir  = '.'
blddir  = 'target'

def set_options(opt):
    wafUtils.setBaseOptions(opt)
    opt.add_option('--doxygen', action='store_true', default=False,
        help='Run doxygen (if able to)')

def configure(conf):
    wafUtils.doBaseConfigure(conf)
    
def pre_build():
    wafUtils.setupVariants()

def build(bld):
    bld.add_subdirs('nitf cgm')
    
