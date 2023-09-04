configure_file("docbook-fo.xslt.in" "${CMAKE_CURRENT_BINARY_DIR}/docbook-fo.xslt")

add_docbook(TARGET manual
            DOCUMENT manual/manual.xml
            SOURCES manual/pattern/fileformat.xml)
