// -*- C++ -*-
//
// $Id$
//

#ifndef _domx_XmlObjectCatalog_h_
#define _domx_XmlObjectCatalog_h_

#include <string>
#include <vector>
#include <set>

namespace domx
{

  class XmlObjectInterface;
  class XmlObjectCatalogP;
  class XmlObjectCatalog;

  /**
   * An XmlObjectCatalog is a cheap but reliable storage mechanism for
   * XmlObject implementations.  It uses the atomic operations of the
   * filesystem to insert and remove object instances.  Instances are
   * written to individual files as xml, and text key for each object is
   * also the filename, which makes it easy to lookup object instances for
   * a given name.  If a method fails, an error message is accumulated and
   * the method returns false.  The current accumulation of error messages
   * can be checked with the errorsPending(), clearErrors(), and
   * getErrors() methods.  The maximum of number of errors accumulated is
   * @code MAX_PENDING_ERRORS.
   **/
  class XmlObjectCatalog
  {
  public:

    typedef std::set<std::string> key_set_t;

    static const unsigned int MAX_PENDING_ERRORS = 10;

    /**
     * Construct a closed, empty catalog.  Nothing can be inserted until a
     * call to open() succeeds.  Until the catalog appears to be empty.
     **/
    XmlObjectCatalog();

    /**
     * Every XmlObjectCatalog has a name.  The directory path @dir is the
     * path under which the catalog's directory will be created.  Thus the
     * full path to the catalog directory will be "<dir>/<name>.catalog".
     * If the directory does not already exist, it will be created.
     *
     * An XmlObjectCatalog can register itself with a system-wide catalog
     * of Catalogs so that file catalogs can be looked up by name without
     * knowing the full directory path.  If @p system_wide is @code true,
     * then this catalog will be registered in the system catalog as long
     * as its directory can be opened successfully.  If an entry by this
     * name already exists in the system catalog, then it will be
     * overwritten.
     *
     * The system catalog is in /var/tmp/system.catalog.
     *
     * To open an existing catalog by its name alone, use the other
     * open() method.
     **/
    bool
    open (const std::string& name, const std::string& dir,
	  bool system_wide = true);

    /**
     * Open the catalog with this name by looking it up in the system
     * catalog.  If no such catalog exists, then this method returns false,
     * isOpen() will return false, and all attempts to change the catalog
     * will fail.  If the system catalog has an entry by this name but its
     * directory does not exist yet, then this method will attempt to
     * create it.  If the directory does not exist and cannot be created,
     * then return false.
     **/
    bool
    open (const std::string& name);

    /**
     * Register this catalog with the system catalog, but only if this
     * catalog is currently open.  If the system catalog cannot be opened,
     * then this method fails.  This will overwrite any existing entry in
     * the catalog by the same name.
     **/
    bool
    registerCatalog();

    /**
     * Return the name of this catalog.
     **/
    std::string
    name();

    /**
     * Check whether the catalog directory could be opened.  If not, this
     * method will return false, and attempts to insert into the catalog
     * will fail without an error message.  The intent is that a
     * nonexistent or unopened catalog just looks like an empty catalog.
     **/
    bool
    isOpen();

    /**
     * Insert the given @p object into this catalog under the given @p name.
     **/
    bool
    insert (const std::string& name, XmlObjectInterface* object);

    /**
     * Remove the object named @p name from this catalog.  This method
     * still returns true if the object does not exist.  It only returns
     * false if an attempt to remove an existing object fails.
     **/
    bool
    remove (const std::string& name);

    /**
     * Move the object with the given @p name from this catalog to @p dest.
     * Note this uses the filesystem atomic rename() call, so the move
     * either succeeds or the object remains in this catalog.  If the
     * move fails, return false and queue the reason for the failure
     * in the error queue.
     **/
    bool
    move (const std::string& name, XmlObjectCatalog* dest);

    /**
     * This loads the given @p object from key @p name in this catalog.
     * See XmlObjectInterface::load().  Return false and leave @p object
     * unchanged if the key does not exist or an error occurred, otherwise
     * return true.
     **/
    bool
    load (const std::string& name, XmlObjectInterface* object);

    /**
     * Return the set of keys in this catalog.  This is a snapshot of the
     * keys made when this method is called, and the set of keys will not
     * be synchronized with any changes made to the catalog directory
     * after this call.  To refresh the set of keys, call this method
     * again.  Since objects may be removed after keys() is called, 
     * an application should be prepared for keys in the key list to
     * be missing when an attempt is made to load them.  Likewise be aware
     * that objects which are inserted into the catalog after calling keys()
     * will not appear in the key list until keys() is called again.
     *
     * The list serves as an ordered index on the objects in the catalog.
     * The order of the keys is lexicographic, since they are strings and
     * use the standard sort order for strings.  Thus it is up to the
     * application to format the keys to give the order it needs, such as
     * prepending the key with a time string which also sorts in time
     * order.  Also note that when different applications will insert
     * objects into a catalog and the ordering of those objects matters,
     * the formatting of the keys must be consistent between applications
     * to get consistent ordering of the index.
     *
     * @returns false if an error occurs loading the keys, otherwise
     * returns true.  The given set will be modified even if the method
     * ultimately fails.
     **/
    bool
    keys(key_set_t& kset);

    /**
     * Return true if an object by this name exists in this catalog,
     * otherwise return false.  The object is not instantiated, so this can
     * be faster than testing with load().  If there is an error trying to
     * check for existence, then this method returns false and adds a
     * message to the error queue.
     **/
    bool
    exists (const std::string& name);

    /**
     * Return the number of accumulated error messages.
     **/
    int
    errorsPending();

    /**
     * Return the current accumulation of error messages.
     **/
    std::vector<std::string>
    getErrors();

    /**
     * Clear the current accumulation of error messages.  After this call
     * errorsPending() will return zero.
     **/
    void
    clearErrors();

    /**
     * Return the most recent error message, or an empty string if there
     * are no errors.
     **/
    std::string
    lastError();

    ~XmlObjectCatalog();

  private:

    friend class XmlObjectCatalogP;

    XmlObjectCatalogP* _mp;

    XmlObjectCatalog& 
    operator= (const XmlObjectCatalog&);

    XmlObjectCatalog (const XmlObjectCatalog&);

  };

}

#endif // _domx_XmlObjectCatalog_h_

