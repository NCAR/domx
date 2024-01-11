# Domx Library

The domx library provides extensions and convenience wrappers for the XML DOM
API, including std::string wrappers for Xerces-C dynamic character arrays and
object encapsulation backed by file storage in xml format.

[XmlObject](domx/XmlObjectNode.h) members can be changed and queried using a
natural interface, but the object state can be loaded and stored with XML
files at any time. Objects can also be extended with state from other object
interfaces, so that a single object can carry with it state from multiple
interfaces. For example, the basic [XmlFileObject](domx/XmlFileObject.h)
contains members like the time attributes, checksum, name, path, and other
attributes of a file. A subclass of XmlFileObject for saving archive file
information can add attributes like the number of blocks, the start time of
the archive session, and the archiver name. That state is always carried with
all instantiations of the file object, even if it is only loaded and
manipulated as a XmlFileObject by more generic tools.  The
[XmlObjectCatalog](domx/XmlObjectCatalog.h) creates a simple database of xml
objects with atomic transactions using the filesystem. Any XmlObject,
especially file objects, can be added to a catalog.

The domx module includes the programs `xmlfilescan` and `xmlcatalog` for
creating and manipulating the XmlCatalog databases from the command line.
They are not built by default.

For simply usage examples, see the test program in `domx/tests`.

## Building Domx

Domx builds require [logx](https://github.com/NCAR/logx) and the Xerces-C C++
library.

Usually `domx` is built as a subdirectory of a larger project, where
[eol_scons](https://github.com/NCAR/eol_scons) finds and loads the
[tool_domx.py](tool_domx.py) to add the library to the build.

To build `domx` from its own directory, use `scons -f SConscript`.  When this
directory is the top of the SCons build, install targets are added for the
library, headers, and utility programs.  Set `OPT_PREFIX` to find `logx`, and
set `INSTALL_PREFIX` to set the install location.  The available settings and
their values can be seen with `scons -f SConscript -h`.  Set the values on the
command-line or in a file `config.py`.  This example builds against `logx`
installed in `/opt` and then installs `domx` under `/opt`.

```sh
scons -f SConscript OPT_PREFIX=/opt INSTALL_PREFIX=/opt install
```
