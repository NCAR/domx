
lib_LIBRARIES = libdomx.a

libdomx_a_SOURCES = XML.cc

# libdomx_a_CPPFLAGS = $(LOGX_INCLUDES) $(XERCESC_INCLUDES)
AM_CPPFLAGS = $(all_includes) $(LOGX_INCLUDES) $(XERCESC_INCLUDES)

DOXYGEN_REFERENCES = $(XERCESC_DOXREF)

include ../config/Doxyfile.am

