# -*- python -*-

Import('env')
env = env.Create('domx')
tools = env.Require(Split('PKG_XERCESC PKG_LOGX'))

def PKG_DOMX(env):
        env.Append(LIBPATH= ['#domx',])
        env.Append(LIBS=['domx',])
	env.Apply (tools)

Export('PKG_DOMX')

lib = env.Library('domx', Split("""
 XML.cc XmlObjectInterface.cc XmlObjectCatalog.cc XmlTime.cc
"""))
Default(lib)

SConscript(dirs=['tests'])

