// -*- C++ -*-

#ifndef _domx_XmlTime_h_
#define _domx_XmlTime_h_

#include <time.h>
#include <iosfwd>

namespace domx
{

  /**
   * XmlTime is a wrapper for a unix time_t with an operator 
   * to translate to and from a standard text format, so that
   * XmlTime can be used as a type for an XmlObjectMember.
   **/
  class XmlTime
  {
  public:

    inline
    XmlTime(time_t t = 0) :
      _time (t)
    {}

    /**
     * Convert this time to a string with a standard format that can be
     * translated back to a time by XmlTime objects.
     *
     * The format: YYYY:MM:DD HH:MM:SS
     **/
    std::string
    toString() const;

    /**
     * Inverse of toString().
     **/
    void
    fromString(const std::string& text);

    /**
     * Return a string for this time in a standard key format which will
     * sort lexicographically in time order.  The time format is
     * "YYYYMMDD.HHMMSS".
     **/
    std::string
    key () const;

    inline
    operator const time_t& () const
    {
      return _time;
    }

    inline
    operator time_t& ()
    {
      return _time;
    }

    inline
    XmlTime&
    operator=(time_t t)
    {
      _time = t;
      return *this;
    }

  private:

    time_t _time;

  };


}

/**
 * Writes the time to the output stream in the format "YYYY:MM:DD HH:MM:SS".
 **/
std::ostream&
operator<<(std::ostream& out, const domx::XmlTime& xt);


/**
 * Parses the input stream for a time in the same format as is written
 * by the output operator.
 **/
std::istream&
operator>>(std::istream& in, domx::XmlTime& xt);


#endif // _domx_XmlTime_h_
