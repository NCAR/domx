# -*- python -*-

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

lib = env.Library('domx', sources)
Default(lib)

xmlfilescan = env.Program('xmlfilescan', sources +
			  ["xmlfilescan.cc"])
Default(xmlfilescan)

SConscript(dirs=['tests'])

