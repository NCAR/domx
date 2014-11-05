# -*- python -*-

import os

tools = ['xercesc', 'logx', 'doxygen']
env = Environment(tools = ['default'] + tools)

domxDir = Dir('.').abspath

def domx(env):
    env.Append(LIBS=[env.GetGlobalTarget('libdomx'),])
    env.AppendUnique(CPPPATH = domxDir)
    env.Require (tools)

Export('domx')

sources = Split("""
 XML.cc XmlObjectInterface.cc XmlObjectCatalog.cc XmlTime.cc XmlFileObject.cc
 """)

headers = Split("""
 XML.h         XmlObjectCatalog.h    XmlObjectMember.h  XmlTime.h
 XmlFileObject.h  XmlObjectInterface.h  XmlObjectNode.h
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

SConscript(dirs=['tests'])

env['DOXYFILE_DICT'].update({ "PROJECT_NAME" : "Domx" })
env.Apidocs(sources + headers)
