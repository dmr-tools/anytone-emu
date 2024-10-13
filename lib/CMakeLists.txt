if (UNIX)
  set(PTY_SOURCE pseudoterminalposix.cc)
  set(PTY_HEADER pseudoterminalposix.hh)
elseif(WIN32)
  set(PTY_SOURCE pseudoterminalwindows.cc)
  set(PTY_HEADER pseudoterminalwindows.hh)
endif(UNIX)

set(libanytone-emu_SOURCES
    request.cc response.cc hexdump.cc xmlparser.cc offset.cc
    device.cc ${PTY_SOURCE} logger.cc model.cc image.cc
    d868uv.cc d868uve.cc d878uv.cc d878uv2.cc d578uv.cc d578uv2.cc
    dmr6x2uv.cc dmr6x2uv2.cc
    djmd5.cc djmd5x.cc
    pattern.cc patternparser.cc codeplugpatternparser.cc
    annotation.cc errorstack.cc)
set(libanytone-emu_HEADERS pseudoterminal.hh
    request.hh response.hh hexdump.hh xmlparser.hh offset.hh
    device.hh ${PTY_HEADER} logger.hh model.hh image.hh
    d868uv.hh d868uve.hh d878uv.hh d878uv2.hh d578uv.hh d578uv2.hh
    dmr6x2uv.hh dmr6x2uv2.hh
    djmd5.hh djmd5x.hh
    pattern.hh patternparser.hh codeplugpatternparser.hh
    annotation.hh errorstack.hh)

set_property(SOURCE pseudoterminal.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE request.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE response.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE hexdump.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE xmlparser.hh PROPERTY SKIP_AUTOGET ON)
set_property(SOURCE offset.hh PROPERTY SKIP_AUTOGET ON)
set_property(SOURCE errorstack.hh PROPERTY SKIP_AUTOGET ON)

configure_file(config.hh.in ${PROJECT_BINARY_DIR}/lib/config.hh)

qt_add_library(libanytone-emu STATIC ${libanytone-emu_SOURCES})
set_target_properties(libanytone-emu PROPERTIES
	MACOSX_RPATH TRUE
  OUTPUT_NAME anytone-emu
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}")
target_link_libraries(libanytone-emu PRIVATE Qt6::Core Qt6::SerialPort)

install(TARGETS libanytone-emu DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${libanytone-emu_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)
install(FILES ${PROJECT_BINARY_DIR}/lib/config.hh DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)

