
#include "XmlFileObject.h"


using namespace domx;


extern "C" 
{
#include "MD5.c"
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

