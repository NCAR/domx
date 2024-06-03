// 
// $Id$
//

#include "domx/XmlObjectCatalog.h"
#include "domx/XmlObjectNode.h"

#include "logx/Logging.h"
#include "logx/system_error.h"
#include "logx/EventSource.h"
#include <stdio.h>
#include <unistd.h>    // for unlink()
#include <sys/errno.h>
#include <sys/types.h>
#include <dirent.h>

#include <fstream>
#include <sstream>

LOGGING("XmlObjectCatalog");

using namespace domx;
using std::string;

using logx::system_error;

namespace domx
{

  struct XmlObjectCatalogP : public XmlObjectInterface
  {
    XmlObjectNode* _xi;
    XmlObjectMember<string> _name;
    XmlObjectMember<string> _path;
    XmlObjectCatalog* _that;

    // Note that root is not preserved, so that root can change as the
    // catalog is migrated between systems, and since specifying root would
    // be superfluous, as its already known by the time the catalog is
    // opened.
    string _root;
    logx::EventSource<XmlObjectCatalogP> failures;

    // Current state, which determines which operations are allowed.
    enum { CLOSED, OPEN } _state;

    XmlObjectCatalogP (XmlObjectCatalog* that) :
      _xi (newNode("xmlobjectcatalog")),
      _name (_xi, "name"),
      _path (_xi, "path"),
      _that (that),
      failures (this, &XmlObjectCatalogP::fail)
    {
      _state = CLOSED;
    }
  
    void
    setPath (const std::string& path)
    {
      // Keep the fully qualified path as the name.
      _path = path;
      _name = path;
      _root = XmlObjectCatalog::rootCatalogDirectory();
    }

    bool
    isOpen ()
    {
      return _state == OPEN;
    }

    void
    fail (const std::string& msg)
    {
      // For now we only log the failure and queue it.  Someday
      // we could enable exceptions on failures.
      //
      ELOG << msg;
      if (_errors.size() >= _that->MAX_PENDING_ERRORS)
      {
	std::vector<string>::iterator it =_errors.begin();
	_errors.erase (it, it+(_errors.size()-_that->MAX_PENDING_ERRORS)+1);
      }
      _errors.push_back (msg);
    }

    bool
    verifyDirectory ();

    string
    objectPath(const std::string& id)
    {
      return getDirectory() + "/" + id + ".xml";
    }

    string
    getDirectory()
    {
      return _root + "/" + _path();
    }

    // Error queue.
    std::vector<string> _errors;

  };

}


namespace
{
  std::string ROOT_DIRECTORY;
}


std::string
XmlObjectCatalog::
rootCatalogDirectory()
{
  // Check for an environment variable to override the default, but
  // anything set explicitly with setRootCatalogDirectory() will always
  // take precedence.
  if (ROOT_DIRECTORY.length() == 0)
  {
    ROOT_DIRECTORY = "/var/xmlobjects";
    char* dir = getenv("DOMX_SYSTEM_CATALOG_DIR");
    if (dir)
      ROOT_DIRECTORY = dir;
  }
  return ROOT_DIRECTORY;
}


void
XmlObjectCatalog::
setRootCatalogDirectory(const std::string& dir)
{
  if (dir.length() > 0)
    ROOT_DIRECTORY = dir;
}


XmlObjectCatalog::
XmlObjectCatalog()
{
  _mp = new XmlObjectCatalogP (this);
}


bool
XmlObjectCatalog::
open(const std::string& path)
{
  // Make sure the parent catalogs can be opened or created.
  string::size_type slash = path.rfind ('/');
  if (slash != string::npos)
  {
    XmlObjectCatalog parent;
    string pname = path.substr(0, slash);
    if (!parent.open (pname))
    {
      _mp->failures() << "could not open " 
		      << pname << ": " << parent.lastError();
      return false;
    }
  }
  _mp->setPath (path);

  // Try to create this directory or verify that the
  // directory exists, but only this once.  If it doesn't
  // exist after this, then all other operations on the
  // catalog will fail.
  //
  if (_mp->verifyDirectory())
  {
    _mp->_state = XmlObjectCatalogP::OPEN;
    return true;
  }
  return false;
}



bool
XmlObjectCatalog::
open (XmlObjectCatalog& parent, const std::string& name)
{
  if (!parent.isOpen())
  {
    _mp->failures() << "cannot open: " << name
		    << ", parent catalog is not open.";
    return false;
  }
  // Append the given name to the parent's path to get the full path
  // of the catalog being opened.
  return open (parent._mp->_path() + "/" + name);
}


bool
XmlObjectCatalog::
registerCatalog()
{
  XmlObjectCatalog syscat;
  if (! syscat.open  ("system"))
  {
    _mp->failures() << "registering " << name()
		    << ": could not open system catalog: "
		    << syscat.lastError();
    return false;
  }
  if (! syscat.insert (dotName(), _mp))
  {
    _mp->failures() << "registering " << name()
		    << ": insert failed: " << syscat.lastError();
    return false;
  }
  return true;
}



std::string
XmlObjectCatalog::
name()
{
  return _mp->_path();
}


std::string
XmlObjectCatalog::
dotName()
{
  string n = name();
  string::size_type p;
  while ((p = n.find('/')) != string::npos)
  {
    n[p] = '.';
  }
  return n;
}


bool
XmlObjectCatalog::
isOpen()
{
  return _mp->isOpen();
}


bool
XmlObjectCatalogP::
verifyDirectory ()
{
  string spath(getDirectory());
  const char* path = spath.c_str();
  struct stat sbuf;
  int result = ::mkdir (path, 0775);
  if (result == 0)
  {
    DLOG << "created directory: " << path;
  }
  else if (errno != EEXIST)
  {
    failures() << system_error("creating directory", path);
    return false;
  }
  // Make sure the entry that exists is a directory
  if (stat (path, &sbuf) < 0)
  {
    failures() << system_error("checking with stat()", path);
    return false;
  }
  if (! S_ISDIR(sbuf.st_mode))
  {
    failures() << path << " is not a directory!";
    return false;
  }
  return true;
}



bool
XmlObjectCatalog::
insert (const std::string& id, XmlObjectInterface* object)
{
  if (! isOpen())
    return false;

  if (id.length() == 0)
  {
    _mp->failures() << "Cannot insert an object with an emtpy name.";
    return false;
  }

  // Assemble two file names for this object from our path
  // and from the name: one is the final name and one is the
  // temporary name.
  string filepath = _mp->objectPath(id);
  string tmpfilepath = filepath + "-temp";

  std::ofstream out (tmpfilepath.c_str());

  if (! out)
  {
    _mp->failures() << system_error("opening", tmpfilepath);
    return false;
  }
  object->toXML (out);
  out.close();

  // Now we can 'insert' the temporary file into the catalog
  // with the atomic rename() function.
  int result = rename (tmpfilepath.c_str(), filepath.c_str());
  if (result < 0)
  {
    _mp->failures() << system_error("renaming", tmpfilepath);
    unlink (tmpfilepath.c_str());
    return false;
  }
  return true;
}



bool
XmlObjectCatalog::
remove (const std::string& id)
{
  if (! isOpen())
    return true;

  string path = _mp->objectPath(id);
  int result = unlink (path.c_str());
  if (result < 0 && errno != ENOENT)
  {
    _mp->failures() << system_error("unlink", path);
    return false;
  }
  return true;
}


#ifdef notdef
bool
XmlObjectCatalog::
move (const std::string& id, XmlObjectCatalog* dest)
{
  if (! isOpen())
    return false;

  if (! dest->isOpen())
  {
    _mp->failures() << "move: destination catalog '" 
		    << dest->name() << "' is not open.";
    return false;
  }

  return true;
}
#endif


bool
XmlObjectCatalog::
load (const std::string& id, XmlObjectInterface* object)
{
  if (! isOpen())
    return false;

  // Just try to open the file.  If we can't then maybe it's just not there.
  string opath = _mp->objectPath(id);
  std::ifstream in (opath.c_str());
  if (! in)
  {
    if (errno == ENOENT)
      return false;
    _mp->failures() << system_error("loading ", opath);
    return false;
  }
  // Now just load it up.
  std::ostringstream buffer;
  buffer << in.rdbuf();
  return object->fromXML (buffer.str());
}


bool
XmlObjectCatalog::
keys(key_set_t& kset)
{
  kset.erase (kset.begin(), kset.end());
  if (! isOpen())
    return false;

  string path = _mp->getDirectory();
  DIR* dir = opendir (path.c_str());
  if (! dir)
  {
    _mp->failures() << system_error("keys(): opening catalog directory", path);
    return false;
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != 0)
  {
    // Only match ?*.xml entries.
    string dname(entry->d_name);
    if (dname.length() > 4 && dname.substr(dname.length()-4) == ".xml")
      kset.insert(dname.substr(0, dname.length()-4));
  }
  closedir(dir);
  return true;
}


#ifdef notdef
bool
XmlObjectCatalog::
exists (const std::string& name)
{
  if (! isOpen())
    return false;

  return false;
}
#endif


int
XmlObjectCatalog::
errorsPending()
{
  return _mp->_errors.size();
}


std::vector<std::string>
XmlObjectCatalog::
getErrors()
{
  return _mp->_errors;
}


void
XmlObjectCatalog::
clearErrors()
{
  _mp->_errors.erase(_mp->_errors.begin(), _mp->_errors.end());
}


std::string
XmlObjectCatalog::
lastError()
{
  if (errorsPending())
  {
    return _mp->_errors.back();
  }
  return "";
}


XmlObjectCatalog::
~XmlObjectCatalog()
{
  delete _mp;
}


