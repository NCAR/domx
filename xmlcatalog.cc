

// Provide a simple command-line mechanism for scripts or other programs to
// use to enter xml objects into an XmlObjectCatalog.


#include "logx/Logging.h"
#include <stdexcept>
#include "domx/XmlObjectCatalog.h"
#include "domx/XmlObjectInterface.h"
#include <string>
#include <iostream>
#include <iterator>

using namespace domx;
using std::runtime_error;
using std::string;
using std::cerr;
using std::cout;
using std::cin;

LOGGING("xmlcatalog");

class catalog_error : public std::runtime_error
{
public:
  catalog_error (const std::string& catalog, const std::string& what) :
    runtime_error (catalog + ": " + what),
    _catalog (catalog)
  {  }

  virtual
  ~catalog_error() throw ()
  {}

  string _catalog;
};


static void
usage()
{
    cerr << "Need at least one argument, the operation to perform:\n"
	 << "xmlcatalog {insert|fetch} ...\n";
}



int
insert (XmlObjectCatalog* catalog, int argc, char* argv[])
{
  if (argc <= 1)
  {
    cerr << "Need a key for the object on standard input.\n"
	 << "Usage: xmlcatalog insert <catalog> <key>\n";
    return 1;
  }
  string key = argv[1];
  XmlObjectInterface object;
  if (! object.fromXML (std::cin))
  {
    throw runtime_error ("error loading xml object");
  }
  return catalog->insert (key, &object);
}



int
fetch (XmlObjectCatalog* catalog, int argc, char* argv[])
{
  if (argc <= 1)
  {
    cerr << "Need a key for the object to fetch.\n"
	 << "Usage: xmlcatalog fetch <catalog> <key>\n";
    return 1;
  }
  string key = argv[1];
  XmlObjectInterface object;
  if (! catalog->load (key, &object))
  {
    throw runtime_error (string("could not load object with key: ") + key);
  }
  return object.toXML(std::cout);
}



int
keys (XmlObjectCatalog* catalog, int argc, char* /*argv*/[])
{
  if (argc > 1)
  {
    cerr << "Usage: xmlcatalog keys <catalog>\n";
    return 1;
  }
  XmlObjectCatalog::key_set_t kset;
  if (! catalog->keys(kset))
  {
    throw catalog_error (catalog->name(), "loading keys");
  }
  std::ostream_iterator<string,char> oi(cout, "\n");
  std::copy (kset.begin(), kset.end(), oi);
  return 0;
}
  


int 
catalogMethod (int (*next)(XmlObjectCatalog*, int, char**),
	       int argc, char* argv[])
{
  if (argc <= 1)
  {
    throw runtime_error ("need a catalog name");
  }
  string name = argv[1];
  XmlObjectCatalog catalog;
  if (! catalog.open(name))
  {
    throw catalog_error (name, "could not open");
  }
  return (*next)(&catalog, argc-1, argv+1);
}



int
xmlcatalog (int argc, char* argv[])
{
  if (argc <= 1)
  {
    usage();
    return 1;
  }

  string opt = argv[1];
  if (opt == "insert")
  {
    return catalogMethod (insert, argc-1, argv+1);
  }
  else if (opt == "fetch")
  {
    return catalogMethod (fetch, argc-1, argv+1);
  }
  else if (opt == "keys")
  {
    return catalogMethod (keys, argc-1, argv+1);
  }
  else 
  {
    usage();
    throw runtime_error(string("Unrecognized action: ") + argv[1]);
  }

  return 0;
}


int
main(int argc, char*argv[])
{
  try 
  {
    return xmlcatalog (argc, argv);
  }
  catch (const std::exception& x)
  {
    cerr << x.what() << std::endl;
  }
  return 9;
}
