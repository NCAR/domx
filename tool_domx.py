# -*- python -*-

Import('PREFIX')

import os

tools = ['xercesc', 'logx', 'doxygen']
env = Environment(tools = ['default'] + tools)

domxDir = Dir('.').abspath

def domx(env):
    env.Append(LIBS=[env.GetGlobalTarget('libdomx'),])
    env.AppendUnique(CPPPATH = domxDir)
    env.AppendDoxref(doxref[0])
    env.Require(tools)

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

# Create install targets if PREFIX is defined
if not PREFIX and env.has_key('INSTALL_PREFIX'):
  install_lib = env.InstallLibrary(lib)
  install_headers = env.InstallHeaders('domx', headers)

xmlfilescan = env.Program('xmlfilescan', sources +
              ["xmlfilescan.cc"])
Default(xmlfilescan)
if not PREFIX and env.has_key('INSTALL_PREFIX'):
  env.InstallProgram(xmlfilescan)

xmlcatalog = env.Program('xmlcatalog', sources +
             ["xmlcatalog.cc"])
Default(xmlcatalog)
if not PREFIX and env.has_key('INSTALL_PREFIX'):
  env.InstallProgram(xmlcatalog)

env['DOXYFILE_DICT'].update({ "PROJECT_NAME" : "Domx" })
doxref = env.Apidocs(sources + headers)

SConscript(dirs=['tests'])
