# -*- python -*-

import os

Import('env')
env = env.Create('domx')
tools = env.Require(Split('PKG_XERCESC PKG_LOGX'))

def PKG_DOMX(env):
        env.Append(LIBPATH= ['#domx',])
        env.Append(LIBS=['domx',])
	env.Apply (tools)

Export('PKG_DOMX')

sources = Split(
"""
 XML.cc XmlObjectInterface.cc XmlObjectCatalog.cc XmlTime.cc XmlFileObject.cc
""")

headers = Split("""
 XML.h		 XmlObjectCatalog.h    XmlObjectMember.h  XmlTime.h
 XmlFileObject.h  XmlObjectInterface.h  XmlObjectNode.h
 """)

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

