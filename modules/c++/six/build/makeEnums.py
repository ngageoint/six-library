import sys, os
try:
    from configparser import ConfigParser
except:
    from ConfigParser import ConfigParser

from os.path import dirname, join

from StringIO import StringIO


def makeEnums(filenames):
    c = ConfigParser()
    c.optionxform = str
    c.read(filenames)
    
    s = StringIO()
    sections = list(c.sections())
    sections = sorted(sections)
    for enum in sections:
        values = []
        dflt = 0
        for (name, value) in c.items(enum):
            name, value = map(lambda x: x.strip(), [name, value])
            if name == '__default__':
                dflt = value
                try:
                    dflt = int(dflt)
                except:{}
                continue
            try:
                value = int(value)
            except:{}
            values.append((name, value))
        
        #sort by value
        def cmpValues(x, y):
            if x[1] < y[1]:
                return -1
            elif x[1] == y[1]:
                return 0
            return 1
        
        try:
            values = sorted(values, cmp=cmpValues)
        except:{}
        
        if dflt is not None:
            for (name, value) in values:
                if dflt == value:
                    dflt = name
                    break
        if type(dflt) != str:
            dflt = values[0][0]
        
        s.write("""
/*!
 *  \struct %s 
 *
 *  Enumeration used to represent %ss
 */\n""" % (enum, enum))
        s.write('struct %s\n{\n' % enum)
        s.write('    //! The enumerations allowed\n')
        s.write('    enum\n    {\n')
        for (i, (name, value)) in enumerate(values):
            if value is not None:
                s.write('        %s = %s' % (name, value))
            else:
                s.write('        %s' % name)
            if i < len(values) - 1:
                s.write(',')
            s.write('\n')
        s.write('    };\n\n')
        
        s.write('    //! Default constructor\n')
        s.write('    %s(){ value = %s; }\n\n' % (enum, dflt))
        
        s.write('    //! string constructor\n')
        s.write('    %s(std::string s)\n    {\n' % enum)
        for (i, (name, value)) in enumerate(values):
            s.write('        %sif (s == "%s")\n            value = %s;\n' % (i > 0 and 'else ' or '',
                                                               name, name))
        s.write('        else\n            throw except::InvalidFormatException(Ctxt(FmtX("Invalid enum value: %s", s.c_str())));\n')
        s.write('    }\n\n')
        
        s.write('    //! int constructor\n')
        s.write('    %s(int i)\n    {\n        switch(i)\n        {\n' % enum)
        idx = 0
        for (i, (name, value)) in enumerate(values):
            if value is not None:
                idx = value
            s.write('        case %s:\n            value = %s;\n            break;\n' % (idx, name))
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
        for (i, (name, value)) in enumerate(values):
            if value is not None:
                idx = value
            s.write('        case %s:\n            return std::string("%s");\n' % (idx, name))
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
        s.write('    int value;\n\n')
        s.write('};\n')
        
#        print('template<> %s Init::undefined<%s>();' % (enum, enum))
#        print('template<> %s Init::undefined<%s>()\n{\n    return %s::NOT_SET;\n}\n' % (enum, enum, enum))
        
    return s.getvalue()
        
if __name__ == '__main__':
    import re, datetime
    code = makeEnums(join(dirname(__file__), 'enums.txt'))
    code += '\n// code auto-generated %s' % datetime.datetime.now()
    
    f = open(join(dirname(__file__), 'Enums.h.template'))
    template = f.read()
    f.close()
    
    template = re.sub(r'[$][{]CODE[}]', r'%s' % code, template)
    
    f = open(join(dirname(__file__), os.pardir, 'include', 'six', 'Enums.h'), 'w')
    f.write(template)
    f.close()
