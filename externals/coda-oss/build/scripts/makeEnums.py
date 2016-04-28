#!/usr/bin/env python

import sys, os, re
try:
    from configparser import ConfigParser
except:
    from ConfigParser import ConfigParser

from os.path import dirname, join

from StringIO import StringIO


def Bunch(**kw):
    return type('Bunch',(), kw)

def makeEnums(filenames):
    c = ConfigParser()
    c.optionxform = str
    c.read(filenames)
    
    s = StringIO()
    sections = list(c.sections())
    sections = sorted(sections)
    for enum in sections:
        values = Bunch(default=0, prefix='', items=[],
                       supportNoPrefixForStrings=False,
                       toStringNoPrefix=False,
                       constShortcuts=False,
                       cleanPrefix='')
        for (name, value) in c.items(enum):
            name, value = map(lambda x: x.strip(), [name, value])
            if name == '__default__':
                values.default = value
                try:
                    values.default = int(values.default)
                except:{}
            elif name == '__enum_prefix__':
                values.prefix = value
                values.cleanPrefix = re.sub(r'[^\w_]', '_', value)
            elif name == '__string_noprefix__':
                values.supportNoPrefixForStrings = value.lower() == 'true'
            elif name == '__tostring_noprefix__':
                values.toStringNoPrefix = value.lower() == 'true'
            elif name == '__const_shortcuts__':
                values.constShortcuts = value.lower() == 'true'
            else:
                valParts = value.split(',')
                try:
                    value = int(valParts[0])
                except:{}
                names=name.split(',')
                toStringVal = len(valParts) > 1 and valParts[1] or names[0]
                values.items.append(Bunch(names=names, value=value,
                                          toString=toStringVal))
                
        
        def cmpValues(x, y):
            if x.value < y.value:
                return -1
            elif x.value == y.value:
                return 0
            return 1
        
        try:
            values.items = sorted(values.items, cmp=cmpValues)
        except:{}
        
        if values.default is not None:
            for item in values.items:
                if values.default == item.value:
                    values.default = ''.join([values.cleanPrefix, item.names[0]])
                    break
        if type(values.default) != str:
            values.default = values.items[0].value
        
        s.write("""
/*!
 *  \struct %s
 *
 *  Enumeration used to represent %ss
 */\n""" % (enum, enum))
        s.write('struct %s\n{\n' % enum)
        s.write('    //! The enumerations allowed\n')
        s.write('    enum\n    {\n')
        for (i, item) in enumerate(values.items):
            if item.value is not None:
                s.write('        %s%s = %s' % (values.cleanPrefix, item.names[0].replace(' ', '_'), item.value))
            else:
                s.write('        %s%s' % (values.cleanPrefix, item.names[0]))
            if i < len(values.items) - 1:
                s.write(',')
            s.write('\n')
        s.write('    };\n\n')
        
        s.write('    //! Default constructor\n')
        s.write('    %s(){ value = %s; }\n\n' % (enum, values.default.replace(' ', '_')))
        
        s.write('    //! string constructor\n')
        s.write('    %s(std::string s)\n    {\n' % enum)
        i = 0
        for item in values.items:
            for n in item.names:
                names = ['%s%s' % (values.cleanPrefix, n)]
                if values.supportNoPrefixForStrings:
                    names.append(n)
                if values.prefix != values.cleanPrefix:
                    names.append('%s%s' % (values.prefix, n))
                for n in names:
                    s.write('        %sif (s == "%s")\n            value = %s%s;\n' % (i > 0 and 'else ' or '',
                                                                       n, values.cleanPrefix, item.names[0].replace(' ', '_')))
                    i += 1
        s.write('        else\n            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));\n')
        s.write('    }\n\n')
        
        s.write('    //! int constructor\n')
        s.write('    %s(int i)\n    {\n        switch(i)\n        {\n' % enum)
        idx = 0
        for (i, item) in enumerate(values.items):
            if item.value is not None:
                idx = item.value
            s.write('        case %s:\n            value = %s%s;\n            break;\n' % (idx, values.cleanPrefix, item.names[0].replace(' ', '_')))
            try:
                idx += 1
            except:{}
        s.write('        default:\n            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", i)));\n')
        s.write('        }\n    }\n\n')
        
        s.write('    //! destructor\n')
        s.write('    ~%s(){}\n\n' % enum)
        
        s.write('    //! Returns string representation of the value\n')
        s.write('    std::string toString() const\n    {\n        switch(value)\n        {\n')
        idx = 0
        for (i, item) in enumerate(values.items):
            if item.value is not None:
                idx = item.value
            if values.toStringNoPrefix:
                s.write('        case %s:\n            return std::string("%s");\n' % (idx, item.toString))
            else:
                s.write('        case %s:\n            return std::string("%s%s");\n' % (idx, values.prefix, item.toString))
            try:
                idx += 1
            except:{}
        s.write('        default:\n            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %d", value)));\n')
        s.write('        }\n    }\n\n')
        
        s.write('    //! assignment operator\n')
        s.write('    %s& operator=(const %s& o)\n    {\n' % (enum, enum))
        s.write('        if (&o != this)\n        {\n            value = o.value;\n        }\n')
        s.write('        return *this;\n    }\n\n')
        
        s.write('    bool operator==(const %s& o) const { return value == o.value; }\n' % enum)
        s.write('    bool operator!=(const %s& o) const { return value != o.value; }\n' % enum)
        s.write('    bool operator==(const int& o) const { return value == o; }\n')
        s.write('    bool operator!=(const int& o) const { return value != o; }\n')
        s.write('    %s& operator=(const int& o) { value = o; return *this; }\n' % enum)
        s.write('    bool operator<(const %s& o) const { return value < o.value; }\n' % enum)
        s.write('    bool operator>(const %s& o) const { return value > o.value; }\n' % enum)
        s.write('    bool operator<=(const %s& o) const { return value <= o.value; }\n' % enum)
        s.write('    bool operator>=(const %s& o) const { return value >= o.value; }\n' % enum)
        s.write('    operator int() const { return value; }\n')
        s.write('    operator std::string() const { return toString(); }\n\n')
        s.write('    static size_t size() { return %d; }\n\n' % len(values.items))
        s.write('    int value;\n\n')
        s.write('};\n\n')
        
        if values.constShortcuts:
            for (i, item) in enumerate(values.items):
                s.write('const %s %s%s(%s::%s%s);\n' % (enum, values.cleanPrefix, item.names[0],
                                                        enum, values.cleanPrefix, item.names[0]))
            s.write('\n')
        
#        print('template<> %s Init::undefined<%s>();' % (enum, enum))
#        print('template<> %s Init::undefined<%s>()\n{\n    return %s::NOT_SET;\n}\n' % (enum, enum, enum))
        
    return s.getvalue()
        
if __name__ == '__main__':
    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog [options] FILE [OUTPUT]")
    parser.add_option("-t", "--template", dest='template', action='store',
                      help="Specify the template file to use")
    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.print_help()
        sys.exit(1)
    
    import re, datetime
    code = makeEnums(args[0])
    code += '\n// code auto-generated %s' % datetime.datetime.now()
    
    if options.template:
        f = open(options.template)
        template = f.read()
        f.close()
        template = re.sub(r'[$][{]CODE[}]', r'%s' % code, template)
    else:
        template = code
    
    if len(args) > 1:
        f = open(args[1], 'w')
        f.write(template)
        f.close()
    else:
        print(template)
        
