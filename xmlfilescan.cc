// 
// Scan a list of files on the command line and generate XmlFileObjects
// for each one, then insert them into a catalog.
//


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <unistd.h>
#include "XmlFileObject.h"
#include "XmlObjectCatalog.h"

#include <string>

using std::string;

#include "logx/Logging.h"
#include "logx/system_error.h"

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

  char cwdbuf[1024];
  const char* cwd = getcwd(cwdbuf, sizeof(cwdbuf));
  if (!cwd)
  {
    ELOG << "Current working directory path too long";
    exit (1);
  }

  int i = 1;
  while (i < argc)
  {
    string opt(argv[i++]);
    if (opt == "--md5")
      md5 = 1;
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
	 << " --md5     Compute md5 sums for each file and record in catalog."
	 << endl;
    exit (1);
  }

  XmlObjectCatalog catalog;
  if (! catalog.open (argv[i++], ".", false))
  {
    ELOG << "Could not open catalog.";
    exit (1);
  }

  struct stat sbuf;
  for (; i < argc; ++i)
  {
    if (stat (argv[i], &sbuf) < 0)
    {
      ++errors;
      ELOG << logx::system_error("calling stat", argv[i]);
      continue;
    }
    if (! S_ISREG(sbuf.st_mode))
    {
      ++errors;
      ELOG << "not a regular file, skipping: " << argv[i];
      continue;
    }
    XmlFileObject xfo;
    xfo.Size = sbuf.st_size;
    xfo.Description = "generic file scanned by xmlfilescan";
    xfo.Changed = sbuf.st_ctime;
    xfo.Modified = sbuf.st_mtime;
    xfo.Accessed = sbuf.st_atime;

    // Figure out filename components: if path is not absolute,
    // first prepend the current working directory.
    string filepath = argv[i];
    if (filepath[0] != '/')
    {
      filepath = string(cwd) + '/' + filepath;
    }
    // Now parse out the directory and filename.
    string dirname = filepath;
    string filename = filepath;
    string::size_type n = filepath.rfind ('/');
    filename.erase (0, n+1);
    dirname.erase (n, string::npos);
    xfo.Name = filename;
    xfo.Directory = dirname;

    // Once the name and path are set, we can ask it to compute its own md5.
    if (md5 && !xfo.computeMD5 ())
    {
      ELOG << "checksum failed for " << string(argv[i]);
      ++errors;
    }
    catalog.insert (xfo.timekey(), &xfo);
  }
  return errors;
}

