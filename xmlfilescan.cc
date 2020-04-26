// 
// Scan a list of files on the command line and generate XmlFileObjects
// for each one, then insert them into a catalog.
//


#include <iostream>
#include <unistd.h>
#include "domx/XmlFileObject.h"
#include "domx/XmlObjectCatalog.h"

#include <string>

using std::string;

#include "logx/Logging.h"

LOGGING("xmlfilescan");

using std::cerr;
using std::cout;
using std::endl;

using domx::XmlFileObject;
using domx::XmlObjectCatalog;

int
main (int argc, char* argv[])
{
  int errors = 0;
  logx::ParseLogArgs (argc, argv);
  int md5 = 0;
  int timekey = 0;

  int i = 1;
  while (i < argc)
  {
    string opt(argv[i++]);
    if (opt == "--md5")
      md5 = 1;
    else if (opt == "--time")
      timekey = 1;
    else
    {
      --i;
      break;
    }
  }

  // First argument must be a catalog name.
  if (argc - i < 2)
  {
    cerr << "Usage: " << argv[0] << " [options]"
	 << " <catalog> <file> [<file> ...]\n";
    cerr << "Options:\n"
	 << " --md5    Compute md5 sums for each file and record in catalog.\n"
	 << " --time   Insert file using its last modified time as the key.\n";
    exit (1);
  }

  XmlObjectCatalog catalog;
  if (! catalog.open (argv[i++]))
  {
    ELOG << "Could not open catalog.";
    exit (1);
  }

  for (; i < argc; ++i)
  {
    XmlFileObject xfo;
    if (! xfo.scan(argv[i]))
    {
      ++errors;
      continue;
    }
    if (md5 && !xfo.computeMD5 ())
    {
      ELOG << "checksum failed for " << string(argv[i]);
      ++errors;
    }
    string key = xfo.Name();
    if (timekey)
      key = xfo.modifiedTimeKey();
    catalog.insert (key, &xfo);
  }
  return errors;
}

