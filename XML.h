// -*- mode: C++; c-basic-offset: 4; -*-
//
// $Id$

#ifndef _domx_XML_
#define _domx_XML_

#include <string>

// Includes for Xerces-C
//#include <util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

//#include <dom/DOM_NodeList.hpp>
//#include <sax/SAXParseException.hpp>
//#include <sax/ErrorHandler.hpp>

#ifdef notdef
class ostream;
ostream& operator<< (ostream& target, const DOMString& s);

namespace log4cpp
{
    class Category;
}
#endif

// Our extensions to DOM reside in the DOMX namespace.
//
namespace domx
{
#ifdef notdef
    typedef DOMDocument Document;
    typedef DOMNode Node;
    typedef DOMDOMException DOMException;
    typedef DOMDocumentType DocumentType;
    typedef void (*XMLConstructor)(Node &);
    void createFromNode (Node &node);

    bool nodeFindAttribute (DOMNode alist, 
			    DOMString name, string *value = 0);
    bool mapFindAttribute (DOMNamedNodeMap alist, 
			   DOMString name, string *value = 0);

    void Print (ostream &out, DOMNode doc);
#endif


    // A convenience class which constructs a string by transcoding 
    // a DOMString and thus avoids leaking the char array returned
    // by transcode().
    //
    class xstring : public std::string
    {
    public:
	const XMLCh *
	xc()
	{
	    delete mxc;
	    mxc = XMLString::transcode(this->c_str());
	    return mxc;
	}

	operator const XMLCh *()
	{
	    return xc();
	}

	xstring (const XMLCh *xc)
	{
	    mxc = 0;
	    assign (xc);
	}

	xstring (const char s[])
	{
	    mxc = 0;
	    string::operator= (s);
	}

	xstring (const xstring &xs) : string (xs)
	{
	    mxc = 0;
	}

	xstring (const string &xs) : string (xs)
	{
	    mxc = 0;
	}

	xstring &operator= (const XMLCh *xc)
	{
	    return assign (xc);
	}

	xstring &operator= (const xstring &xs)
	{
	    string::operator= (xs);
	    return *this;
	}

	xstring &assign (const XMLCh *xc)
	{
	    char *p = XMLString::transcode(xc);
	    string::operator= (p);
	    delete [] p;
	    return *this;
	}

    private:
	XMLCh *mxc;
    };

};


#endif // _domx_XML_
