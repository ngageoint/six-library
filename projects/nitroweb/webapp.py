import web
from web.contrib.template import render_mako
import os, itertools

import config
from utils import recursiveGlob
import nitf
try:
    import simplejson as json
except:
    import json


render = render_mako(directories=[web.config.baseDir + 'templates'],
                     input_encoding='utf-8',
                     output_encoding='utf-8')

urls = (
    '/list',        'ListView',
    '/file',        'GetFile',
)
app = web.application(urls, globals())


class ListView:
    def GET(self):
        nitfDir = web.config.nitfDir
        
        dirs = []
        for k, g in itertools.groupby(recursiveGlob(nitfDir, patterns=['*.ntf', '*.nsf'], relative=True),
                                  lambda x: os.path.split(x)[0]):
            files = map(lambda x: os.path.split(x)[1], g)
            files.sort()
            dirs.append([k, files])
        dirs.sort()
        web.header('Content-Type', 'text/html')
        return render.list(files=dirs)


class NITFRecord:
    def __init__(self, filename):
        self.handle = nitf.IOHandle(str(filename))
        self.reader = nitf.Reader()
        self.record = self.reader.read(self.handle)
    
    def __listHeader(self, header):
        fields = []
        for f in header:
            #for now, skipping binary... it's causing problems
            if nitf.Field.NITF_BINARY == f[1].getType():
                fields.append((f[0], f[1].getLength(), '<binary data>'))
            else:
                fields.append((f[0], f[1].getLength(), f[1].getString()))
        return fields
            
    
    def getHeader(self):
        return self.__listHeader(self.record.header)
    
    def getImages(self):
        images = []
        for image in self.record.getImages():
            context = {}
            context['header'] = self.__listHeader(image.subheader)
            #TODO add a thumbnail, possibly
            images.append(context)
        return images
    
    def getTexts(self):
        texts = []
        for text in self.record.getTexts():
            context = {}
            context['header'] = self.__listHeader(text.subheader)
            #TODO add the text data content
            texts.append(context)
        return texts

    def getGraphics(self):
        graphics = []
        for g in self.record.getGraphics():
            context = {}
            context['header'] = self.__listHeader(g.subheader)
            graphics.append(context)
        return graphics

    def getDES(self):
        des = []
        for d in self.record.getDataExtensions():
            context = {}
            context['header'] = self.__listHeader(d.subheader)
            des.append(context)
        return des
    
    def toDict(self):
        d = {}
        d['header'] = self.getHeader()
        images = self.getImages()
        if images:
            d['images'] = images
        graphics = self.getGraphics()
        if graphics:
            d['graphics'] = graphics
        texts = self.getTexts()
        if texts:
            d['texts'] = texts
        des = self.getDES()
        if des:
            d['des'] = des
        
        return d


class GetFile:
    def GET(self):
        params = web.input()
        d = params.pop('dir', '.').replace('..', '')
        fname = params.pop('fname', ''). replace('..', '')
        jsonp = params.pop('jsonp', None)
        
        try:
            f = os.path.join(web.config.nitfDir, d, fname)
            record = NITFRecord(f)
            data = record.toDict()
            data.update({'dir': d, 'fname':fname})
            jsonData = json.dumps(data)
            
            web.header('Content-Type', 'application/json')
            if jsonp:
                return '%s(%s);' % (jsonp, jsonData)
            return jsonData
        except Exception, e:
            raise web.badrequest()
        

if __name__ == '__main__': 
    app.run()
