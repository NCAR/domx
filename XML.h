// -*- mode: C++; c-basic-offset: 4; -*-
//
// $Id$

#ifndef _domx_XML_
#define _domx_XML_

#include <string>

// Includes for Xerces-C
#include <util/PlatformUtils.hpp>
#include <util/XercesDefs.hpp>
#include <parsers/DOMParser.hpp>
#include <dom/DOM_DOMException.hpp>
#include <dom/DOM.hpp>
#include <dom/DOM_NodeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/ErrorHandler.hpp>

class ostream;
ostream& operator<< (ostream& target, const DOMString& s);

namespace log4cpp
{
    class Category;
}


// Our extensions to DOM reside in the DOMX namespace.
//
namespace domx
{
    typedef DOM_Document Document;
    typedef DOM_Node Node;
    typedef DOM_DOMException DOMException;
    typedef DOM_DocumentType DocumentType;

    typedef void (*XMLConstructor)(Node &);

    void createFromNode (Node &node);

    bool nodeFindAttribute (DOM_Node alist, 
			    DOMString name, string *value = 0);
    bool mapFindAttribute (DOM_NamedNodeMap alist, 
			   DOMString name, string *value = 0);

    void Print (ostream &out, DOM_Node doc);

    class ErrorFormatter
    {
    public:
	// I purposefully decided to leave these non-const to give subclasses
	// more flexibility, such as allowing each formatted string to include
	// the current error count.  The implication is that we can't use a
	// shared default error formatter in ErrorHandler classes like
	// StreamErrorLogger.
	//
	virtual string warning(const SAXParseException& toCatch);
	virtual string error(const SAXParseException& toCatch);
	virtual string fatalError(const SAXParseException& toCatch);
    };


    class StreamErrorLogger : public ErrorHandler
    {
    public:

	// We take ownership of the ErrorFormatter and will delete it 
	// when destroyed.
	//
	StreamErrorLogger (ErrorFormatter *fmt = new ErrorFormatter());

	// ------------------------------------------------------------------
	//  Implementation of the error handler interface
	// ------------------------------------------------------------------
	void warning(const SAXParseException& toCatch);
	void error(const SAXParseException& toCatch);
	void fatalError(const SAXParseException& toCatch);
	void resetErrors();

	~StreamErrorLogger () { delete mFormat; }

    private:
	StreamErrorLogger (const StreamErrorLogger &);
	StreamErrorLogger &operator= (const StreamErrorLogger &);

	log4cpp::Category &log;
	int mNumFatal;
	int mNumError;
	int mNumWarning;
	ErrorFormatter *mFormat;

    };


    // A convenience class which constructs a string by transcoding 
    // a DOMString and thus avoids leaking the char array returned
    // by transcode().
    //
    class xstring : public std::string
    {
    public:
	xstring (const DOMString &ds)
	{
	    assign (ds);
	}

	xstring (const XMLCh *xc)
	{
	    assign (DOMString (xc));
	}

	xstring &operator= (const DOMString &ds)
	{
	    assign (ds);
	    return *this;
	}

	xstring &assign (const DOMString &ds)
	{
	    char *p = ds.transcode();
	    string::operator= (p);
	    delete [] p;
	    return *this;
	}
    };

};


#endif // _domx_XML_
