# -*- python -*-

Import('env')
env = env.Create('domx')
tools = env.Require('PKG_XERCESC')

def PKG_DOMX(env):
        env.Append(LIBPATH= ['#domx',])
        env.Append(LIBS=['domx',])
	env.Apply (tools)

Export('PKG_DOMX')

lib = env.Library('domx', Split("""
 XML.cc
"""))
Default(lib)
