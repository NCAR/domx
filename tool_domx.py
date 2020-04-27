# -*- python -*-

import os

# Need prefixoptions for Install methods.
tools = ['xercesc', 'logx', 'doxygen', 'prefixoptions']
env = Environment(tools = ['default'] + tools)

domxdir = Dir('.')

def domx(env):
    env.Append(LIBS=[env.GetGlobalTarget('libdomx'),])
    env.AppendUnique(CPPPATH = domxdir)
    env.AppendDoxref(doxref[0])
    env.Require(tools)

Export('domx')

sources = Split("""
 XML.cc XmlObjectInterface.cc XmlObjectCatalog.cc XmlTime.cc XmlFileObject.cc
 """)

headers = Split("""
 domx/XML.h domx/XmlObjectCatalog.h domx/XmlObjectMember.h domx/XmlTime.h
 domx/XmlFileObject.h domx/XmlObjectInterface.h domx/XmlObjectNode.h
 domx/XmlFileReference.h domx/XmlObjectReference.h domx/domxfwd.h
 """)

env.SourceCode('.', None)

lib = env.Library('domx', sources)
Default(lib)
install_lib = env.InstallLibrary(lib)
install_headers = env.InstallHeaders('domx', headers)

xmlfilescan = env.Program('xmlfilescan', sources +
              ["xmlfilescan.cc"])
Default(xmlfilescan)
env.InstallProgram(xmlfilescan)

xmlcatalog = env.Program('xmlcatalog', sources +
             ["xmlcatalog.cc"])
Default(xmlcatalog)
env.InstallProgram(xmlcatalog)

env['DOXYFILE_DICT'].update({ "PROJECT_NAME" : "Domx" })
doxref = env.Apidocs(sources + headers)

SConscript(dirs=['tests'])
