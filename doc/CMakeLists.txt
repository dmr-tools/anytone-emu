configure_file("docbook-fo.xslt.in" "${CMAKE_CURRENT_BINARY_DIR}/docbook-fo.xslt")

if (BUILD_MANUAL)
  add_docbook(TARGET manual
    DOCUMENT manual/manual.xml
    SOURCES manual/pattern/fileformat.xml)
endif()

if (BUILD_API_DOCS)
  set(DOXYGEN_PROJECT_NAME "libanytone-emu")
  set(DOXYGEN_PROJECT_BRIEF "A library emulate and reverse engineer AnyTone radios.")
  set(DOXYGEN_BRIEF_MEMBER_DESC YES)
  set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
  set(DOXYGEN_DOTFILE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/fig")
  set(DOXYGEN_ENABLE_PREPROCESSING YES)
  set(DOXYGEN_MACRO_EXPANSION YES)
  set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
  set(DOXYGEN_PREDEFINED __attribute__(x) =)
  set(DOXYGEN_IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/fig")
  doxygen_add_docs(anytone-emu-api-docs
    ${CMAKE_SOURCE_DIR}/lib
    ALL)
endif()
