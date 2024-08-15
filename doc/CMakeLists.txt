configure_file("docbook-fo.xslt.in" "${CMAKE_CURRENT_BINARY_DIR}/docbook-fo.xslt")

if (BUILD_MANUAL)
  add_docbook(TARGET manual
    DOCUMENT manual/manual.xml
    SOURCES manual/pattern/fileformat.xml)
endif()

if (BUILD_API_DOCS)
  doxygen_add_docs(anytone-emu-api-docs
    ${CMAKE_SOURCE_DIR}/lib)
endif()
