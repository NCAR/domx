# -*- python -*-

from SCons.Script import Environment, Export, SConscript

# Need prefixoptions for Install methods.
tools = ['xercesc', 'logx', 'doxygen', 'prefixoptions']
env = Environment(tools=['default'] + tools)

domxdir = env.Dir('.')

sources = env.Split("""
 XML.cc XmlObjectInterface.cc XmlObjectCatalog.cc XmlTime.cc XmlFileObject.cc
""")

headers = env.Split("""
 domx/XML.h domx/XmlObjectCatalog.h domx/XmlObjectMember.h domx/XmlTime.h
 domx/XmlFileObject.h domx/XmlObjectInterface.h domx/XmlObjectNode.h
 domx/XmlFileReference.h domx/XmlObjectReference.h domx/domxfwd.h
""")

lib = env.Library('domx', sources)
env.Default(lib)

xmlfilescan = env.Program('xmlfilescan', sources + ["xmlfilescan.cc"])
xmlcatalog = env.Program('xmlcatalog', sources + ["xmlcatalog.cc"])

env['DOXYFILE_DICT'].update({"PROJECT_NAME": "Domx"})
env.Apidocs(sources + headers)


def domx(env):
    env.Append(LIBS=lib)
    env.AppendUnique(CPPPATH=domxdir)
    env.Require(tools)


Export('domx')

SConscript(dirs=['tests'])

if domxdir == env.Dir('#'):
    install = env.InstallLibrary(lib)
    install += env.InstallHeaders('domx', headers)
    install += env.InstallProgram(xmlfilescan)
    install += env.InstallProgram(xmlcatalog)
    env.Alias('install', install)
    env.SetHelp()
