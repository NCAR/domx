
lib_LIBRARIES = libdomx.a

libdomx_a_SOURCES = \
    XML.cc XmlObjectInterface.cc \
    XmlObjectCatalog.cc XmlTime.cc XmlFileObject.cc \
    XmlObjectReference.cc XmlFileReference.cc

# libdomx_a_CPPFLAGS = $(LOGX_INCLUDES) $(XERCESC_INCLUDES)
AM_CPPFLAGS = $(all_includes) $(LOGX_INCLUDES) $(XERCESC_INCLUDES)

bin_PROGRAMS = xmlcatalog xmlfilescan
xmlcatalog_SOURCES = xmlcatalog.cc
xmlcatalog_LDADD   = $(DOMX_LIBS)
xmlfilescan_SOURCES = xmlfilescan.cc
xmlfilescan_LDADD   = $(DOMX_LIBS)

DOXYGEN_REFERENCES = $(XERCESC_DOXREF)

include ../config/Doxyfile.am

