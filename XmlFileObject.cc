
#include "domx/XmlFileObject.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string>

#include "logx/Logging.h"
#include "logx/system_error.h"

LOGGING("XmlFileObject");

using namespace domx;
using std::string;

extern "C" 
{
#include "MD5.c"
}

namespace domx
{
  struct MD5Private
  {
    MD5Context ctx;
  };
}

XmlFileObject::
XmlFileObject() :
  _xi(newNode("xmlfileobject")),
  _md5context(0),
  Name (_xi, "filename"),
  Description (_xi, "description"),
  Directory (_xi, "directory"),
  Size (_xi, "size"),
  Created (_xi, "created"),
  Modified (_xi, "modified"),
  Changed (_xi, "changed"),
  Accessed (_xi, "accessed"),
  MD5 (_xi, "md5"),
  State (_xi, "state", CLOSED),
  Expires (_xi, "expires")
{}


XmlFileObject::
~XmlFileObject()
{
  delete _md5context;
}


void
XmlFileObject::
setPath (const std::string& target)
{
  // Figure out filename components: if path is not absolute,
  // first prepend the current working directory.
  string filepath = target;
  if (filepath[0] != '/')
  {
    char cwdbuf[1024];
    const char* cwd = getcwd(cwdbuf, sizeof(cwdbuf));
    if (!cwd)
    {
      ELOG << "Current working directory path too long";
      return;
    }
    filepath = string(cwd) + '/' + filepath;
  }
  // Now parse out the directory and filename.
  string dirname = filepath;
  string filename = filepath;
  string::size_type n = filepath.rfind ('/');
  filename.erase (0, n+1);
  dirname.erase (n, string::npos);
  Name = filename;
  Directory = dirname;
}



int
XmlFileObject::
computeMD5 ()
{
  char zbuf[33];
  std::string path = fullpath();
  if (MD5File (path.c_str(), zbuf) < 0)
  {
    return 0;
  }
  this->MD5 = zbuf;
  return 1;
}


void
XmlFileObject::
updateMD5 (unsigned char* buf, unsigned int len)
{
  if (! _md5context)
  {
    _md5context = new MD5Private;
    MD5Init (&_md5context->ctx);
  }
  MD5Update (&_md5context->ctx, buf, len);
}


void
XmlFileObject::
finishMD5 ()
{
  if (_md5context)
  {
    unsigned char digest[16];
    char base16[33];

    MD5Final(digest, &_md5context->ctx);
    DigestToBase16(digest, base16);
    this->MD5 = base16;
    delete _md5context;
    _md5context = 0;
  }
}


int
XmlFileObject::
scan (const std::string& target)
{
  setPath (target);
  Description = "";
  // Since the creation time of this file is unknown, force it to zero.
  Created = 0;
  return scan();
}


int
XmlFileObject::
scan ()
{
  std::string target = fullpath();
  struct stat sbuf;
  if (stat (target.c_str(), &sbuf) < 0)
  {
    ELOG << logx::system_error("calling stat", target);
    return 0;
  }
  if (! S_ISREG(sbuf.st_mode))
  {
    ELOG << "not a regular file, skipping: " << target;
    return 0;
  }
  Size = sbuf.st_size;
  Changed = sbuf.st_ctime;
  Modified = sbuf.st_mtime;
  Accessed = sbuf.st_atime;
  return 1;
}


void
XmlFileObject::
setSecondsUntilExpires(int seconds)
{
  Expires = time(0) + seconds;
}

