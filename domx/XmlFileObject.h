// -*- C++ -*-
//
// $Id$
//
#ifndef _domx_XmlFileObject_h_
#define _domx_XmlFileObject_h_

#include "XmlObjectNode.h"
#include "XmlTime.h"

#include <string>

namespace domx
{

  struct MD5Private;

  class XmlFileObject : public XmlObjectInterface
  {
    XmlObjectNode* _xi;
    MD5Private* _md5context;

  public:

    typedef enum { OPEN, CLOSED } EnumFileState;

    struct StateStorage : public EnumStorage<EnumFileState>
    {
      StateStorage() { enums[OPEN] = "open"; enums[CLOSED] = "closed"; }
    };

    XmlFileObject();

    ~XmlFileObject();

    /**
     * The name component of the file path.
     **/
    XmlObjectMember<std::string> Name;

    /**
     * A description of this file.
     **/
    XmlObjectMember<std::string> Description;


    /**
     * Given a directory path, set the Directory and Name fields
     * accordingly.  If the path is relative (does not start with /), then
     * the current working directory will be prepended before splitting the
     * full path into directory and filename components.
     **/
    void
    setPath (const std::string& path);

    /**
     * The directory component of the file path.
     **/
    XmlObjectMember<std::string> Directory;

    /**
     * The size of the file, which may or may not be set by the
     * application.
     **/
    XmlObjectMember<unsigned long> Size;

    /**
     * The time the file was created, if known.
     **/
    XmlObjectMember<XmlTime> Created;

    /**
     * Stat attributes for the file.
     **/
    XmlObjectMember<XmlTime> Modified;
    XmlObjectMember<XmlTime> Changed;
    XmlObjectMember<XmlTime> Accessed;

    /**
     * An md5sum for the file, stored as a 32-character base16 string.
     **/
    XmlObjectMember<std::string> MD5;

    /**
     * For programs generating files, they need a way to record the
     * existence of the file in the database but mark it as still under
     * construction and not finished yet.  Thus a file can have state:
     * 'open' or 'closed'.
     **/
    XmlObjectMember<EnumFileState, StateStorage> State;

    /**
     * Change the state to closed.
     **/
    void
    setClosed()
    {
      State = CLOSED;
    }

    /**
     * Change the state to open.
     **/
    void
    setOpen()
    {
      State = OPEN;
    }

    /**
     * The expiration time of the file.  The archiver guarantees that it
     * will have closed the file by this time, in case it fails to close it
     * cleanly and record the closing because of a crash.  Consumers of the
     * files, such as the tape tar handler, can use this to know when to
     * consider a file 'done' if it was not actually recorded as closed.
     **/
    domx::XmlObjectMember<domx::XmlTime> Expires;

    /**
     * Convenience method to set the expiration time to some number of
     * seconds from now.
     **/
    void
    setSecondsUntilExpires(int seconds);

    /**
     * Generate a key from this file's timetag and filename so that
     * keys will be sorted in time order by XmlObjectCatalog but
     * will still be unique if different files are inserted with the
     * same time.
     **/
    std::string
    timekey()
    {
      return Created().key() + "-" + Name();
    }

    /**
     * Generate a key from this file's timetag and filename so that
     * keys will be sorted in time order by XmlObjectCatalog but
     * will still be unique if different files are inserted with the
     * same time.
     **/
    std::string
    modifiedTimeKey()
    {
      return Modified().key() + "-" + Name();
    }

    /**
     * Return the full path to this file.
     **/
    std::string
    fullpath()
    {
      return Directory() + "/" + Name();
    }
      

    /**
     * Scan the file at the file path @p target and fill in this file
     * object from it.  This does not compute the MD5 checksum, that must
     * be requested separately with computeMD5().
     **/
    int
    scan (const std::string& target);


    /**
     * Scan the current file path and fill in fields which can be queried
     * with a stat() call, such as access and modified times and the size.
     * This is useful when a program has finished writing a file and wants
     * the final values to be updated from the filesystem.
     **/
    int
    scan ();


    /**
     * Compute the md5 checksum for this file using the current directory
     * and filename, and store that checksum into the MD5 attribute.
     *
     * @returns zero on failure, non-zero otherwise.
     **/
    int
    computeMD5 ();


    /**
     * Update a running md5 checksum for this file by passing the block
     * of bytes which have been appended to the file.  This does not set the
     * the checksum attribute of this file object, only updates the running
     * computation.  If no running checksum has been started yet, then it
     * will be initialized the first time this method is called.  To
     * compute the final value of the running checksum and set that value
     * on this file, call finishMD5().
     * 
     **/
    void
    updateMD5 (unsigned char* buf, unsigned int len);


    /**
     * Complete the calculation of the running MD5 checksum and set that
     * checksum on this file object.  Subsequent calls to updateMD5() will
     * start over as if an empty data stream.  If no running checksum has
     * been started with updateMD5(), then this method does nothing.
     **/
    void
    finishMD5 ();

  };


}



#endif // _domx_XmlFileObject_h_

