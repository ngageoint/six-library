#! /usr/bin/env python
# encoding: UTF-8
# Petar Forai
# Thomas Nagy 2008-2010 (ita)

import re
from waflib import Task, Utils, Logs, Options
from waflib.TaskGen import extension, feature, after_method
from waflib.Configure import conf
from waflib.Tools import c_preproc

"""
tasks have to be added dynamically:
- swig interface files may be created at runtime
- the module name may be unknown in advance
"""

SWIG_EXTS = ['.swig', '.i']

re_module = re.compile('%module(?:\s*\(.*\))?\s+(.+)', re.M)

re_1 = re.compile(r'^%module.*?\s+([\w]+)\s*?$', re.M)
re_2 = re.compile('%include "(.*)"', re.M)
re_3 = re.compile('#include "(.*)"', re.M)

class swig(Task.Task):
    color   = 'BLUE'
    run_str = '${SWIG} ${SWIGFLAGS} ${SWIGPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${SRC}'
    ext_out = ['.h'] # might produce .h files although it is not mandatory

    def runnable_status(self):
        for t in self.run_after:
            if not t.hasrun:
                return Task.ASK_LATER

        if not getattr(self, 'init_outputs', None):
            self.init_outputs = True
            if not getattr(self, 'module', None):
                # search the module name
                txt = self.inputs[0].read()
                m = re_module.search(txt)
                if not m:
                    raise ValueError("could not find the swig module name")
                self.module = m.group(1)

            swig_c(self)

            # add the language-specific output files as nodes
            # call funs in the dict swig_langs
            for x in self.env['SWIGFLAGS']:
                # obtain the language
                x = x[1:]
                try:
                    fun = swig_langs[x]
                except KeyError:
                    pass
                else:
                    fun(self)

        return super(swig, self).runnable_status()

    def post_run(self):
        if self.swig_install_fun is not None:
            self.swig_install_fun(self)

        super(swig, self).post_run()

    def scan(self):
        "scan for swig dependencies, climb the .i files"
        env = self.env

        lst_src = []

        seen = []
        to_see = [self.inputs[0]]

        while to_see:
            node = to_see.pop(0)
            if node in seen:
                continue
            seen.append(node)
            lst_src.append(node)

            # read the file
            code = node.read()
            code = c_preproc.re_nl.sub('', code)
            code = c_preproc.re_cpp.sub(c_preproc.repl, code)

            # find .i files and project headers
            names = re_2.findall(code) + re_3.findall(code)
            for n in names:
                for d in self.generator.includes_nodes + [node.parent]:
                    u = d.find_resource(n)
                    if u:
                        to_see.append(u)
                        break
                else:
                    Logs.warn('could not find %r' % n)

        return (lst_src, [])

# provide additional language processing
swig_langs = {}
def swigf(fun):
    swig_langs[fun.__name__.replace('swig_', '')] = fun
swig.swigf = swigf

def swig_c(self):
    ext = '.swigwrap_%d.c' % self.generator.idx
    flags = self.env['SWIGFLAGS']
    if '-c++' in flags:
        ext += 'xx'
    out_node = self.inputs[0].parent.find_or_declare(self.module + ext)

    if '-c++' in flags:
        c_tsk = self.generator.cxx_hook(out_node)
    else:
        c_tsk = self.generator.c_hook(out_node)

    c_tsk.set_run_after(self)

    ge = self.generator.bld.producer
    ge.outstanding.insert(0, c_tsk)
    ge.total += 1

    try:
        ltask = self.generator.link_task
    except AttributeError:
        pass
    else:
        ltask.set_run_after(c_tsk)
        ltask.inputs.append(c_tsk.outputs[0])

    self.outputs.append(out_node)

    if not '-o' in self.env['SWIGFLAGS']:
        self.env.append_value('SWIGFLAGS', ['-o', self.outputs[0].abspath()])

@swigf
def swig_python(tsk):
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + '.py'))

@swigf
def swig_ocaml(tsk):
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + '.ml'))
    tsk.set_outputs(tsk.inputs[0].parent.find_or_declare(tsk.module + '.mli'))

@extension(*SWIG_EXTS)
def i_file(self, node):
    # the task instance
    tsk = self.create_task('swig')
    tsk.set_inputs(node)
    tsk.module = getattr(self, 'swig_module', None)

    flags = self.to_list(getattr(self, 'swig_flags', []))
    tsk.env.append_value('SWIGFLAGS', flags)

    tsk.swig_install_fun = getattr(self, 'swig_install_fun', None)

    # looks like this is causing problems
    #if not '-outdir' in flags:
    #    tsk.env.append_value('SWIGFLAGS', ['-outdir', node.parent.abspath()])

@conf
def check_swig_version(self, minver=None):
    """Check for a minimum swig version like conf.check_swig_version('1.3.28')
    or conf.check_swig_version((1,3,28)) """
    reg_swig = re.compile(r'SWIG Version\s(.*)', re.M)
    swig_out = self.cmd_and_log('"%s" -version' % self.env['SWIG'])

    swigver = [int(s) for s in reg_swig.findall(swig_out)[0].split('.')]
    if isinstance(minver, basestring):
        minver = [int(s) for s in minver.split(".")]
    if isinstance(minver, tuple):
        minver = [int(s) for s in minver]
    result = (minver is None) or (minver[:3] <= swigver[:3])
    swigver_full = '.'.join(map(str, swigver))
    if result:
        self.env['SWIG_VERSION'] = swigver_full
    minver_str = minver and '.'.join(map(str, minver))
    if minver is None:
        self.msg('swig version', swigver_full)
    else:
        self.msg('swig version >= %s' % (minver_str,), result, option=swigver_full)
    return result

def options(opt):
    opt.add_option('--enable-swig', action='store_true', dest='swig',
                   help='Disable swig', default=False)
    opt.add_option('--disable-swig', action='store_false', dest='swig',
                   help='Disable swig')
    opt.add_option('--swig-version', action='store', dest='swigver',
                   default=None, help='Specify the minimum swig version')
    opt.add_option('--require-swig', action='store_true', dest='require_swig',
                   help='Require swig (configure option)', default=False)

def configure(conf):
    if Options.options.swig:
        swig = conf.find_program('swig', var='SWIG', mandatory=Options.options.require_swig)
        if not swig: return
        swigver = Options.options.swigver
        if swigver:
            swigver = map(int, swigver.split('.'))
        conf.check_swig_version(minver=swigver)
        conf.env.SWIGPATH_ST = '-I%s'
