// $Id$

// To get strptime from time.h with GLIBC:
#define _XOPEN_SOURCE
#include "XmlTime.h"

#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>

namespace 
{

  time_t
  xt_timegm (struct tm *tm)
  {
    time_t ret;
    char *tz;

    tz = getenv("TZ");
    setenv("TZ", "", 1);
    tzset();
    ret = mktime(tm);
    if (tz)
      setenv("TZ", tz, 1);
    else
      unsetenv("TZ");
    tzset();
    return ret;
  }

}


std::string
domx::XmlTime::
key () const
{
  struct tm tm;
  char buf[32];
  gmtime_r (&_time, &tm);
  strftime (buf, sizeof(buf), "%Y%m%d.%H%M%S", &tm);
  return buf;
}


std::string
domx::XmlTime::
toString() const
{
  struct tm tm;
  gmtime_r (&_time, &tm);
  char buf[32];
  strftime (buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
  return buf;
}


void
domx::XmlTime::
fromString(const std::string& text)
{
  std::istringstream in (text);
  in >> *this;
}


std::ostream&
operator<<(std::ostream& out, const domx::XmlTime& xt)
{
  return out << xt.toString();
}


std::istream&
operator>>(std::istream& in, domx::XmlTime& xt)
{
  struct tm tm;
  std::string dtext, ttext;
  in >> dtext;
  in >> ttext;
  dtext += " ";
  dtext += ttext;
  strptime (dtext.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
  xt = xt_timegm(&tm);
  return in;
}

