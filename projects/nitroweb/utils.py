import sys, os, fnmatch

class GlobDirectoryWalker:
    """ recursively walk a directory, matching filenames """
    def __init__(self, directory, patterns=["*"], relative=False):
        self.stack = [directory]
        self.patterns = patterns
        self.files = []
        self.index = 0
        self.initialDir = directory
        self.relative = relative
        
    def __iter__(self):
        return self.next()
    
    def next(self):
        while True:
            try:
                file = self.files[self.index]
                self.index = self.index + 1
            except IndexError:
                # pop next directory from stack
                if len(self.stack) == 0:
                    raise StopIteration
                self.directory = self.stack.pop()
                if os.path.isdir(self.directory):
                    self.files = os.listdir(self.directory)
                else:
                    self.files, self.directory = [self.directory], ''
                self.index = 0
            else:
                # got a filename
                fullname = os.path.join(self.directory, file)
                if os.path.isdir(fullname) and not os.path.islink(fullname):
                    self.stack.append(fullname)
                for p in self.patterns:
                    if fnmatch.fnmatch(file, p):
                        if self.relative:
                            yield fullname.split(self.initialDir)[1].lstrip('/\\')
                        else:
                            yield fullname

def recursiveGlob(directory, patterns=["*"], relative=False):
    return GlobDirectoryWalker(directory, patterns, relative=relative)

