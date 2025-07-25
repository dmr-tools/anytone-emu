if (UNIX)
  set(PTY_SOURCE pseudoterminalposix.cc)
  set(PTY_HEADER pseudoterminalposix.hh)
elseif(WIN32)
  set(PTY_SOURCE pseudoterminalwindows.cc)
  set(PTY_HEADER pseudoterminalwindows.hh)
endif(UNIX)


set_property(SOURCE pseudoterminal.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE request.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE response.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE hexdump.hh PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE xmlparser.hh PROPERTY SKIP_AUTOGET ON)
set_property(SOURCE offset.hh PROPERTY SKIP_AUTOGET ON)
set_property(SOURCE errorstack.hh PROPERTY SKIP_AUTOGET ON)
set_property(SOURCE modelrom.hh PROPERTY SKIP_AUTOGET ON)

configure_file(config.hh.in ${PROJECT_BINARY_DIR}/lib/config.hh)

qt_add_library(libanytone-emu STATIC ${libanytone-emu_SOURCES}
  hexdump.hh hexdump.cc
  xmlparser.hh xmlparser.cc
  offset.hh offset.cc
  device.hh device.cc
  ${PTY_HEADER} ${PTY_SOURCE}
  logger.hh logger.cc
  model.hh model.cc
  image.hh image.cc
  pattern.hh pattern.cc
  patternparser.hh patternparser.cc
  codeplugpatternparser.hh codeplugpatternparser.cc
  patternfragmentparser.hh patternfragmentparser.cc
  patternmimedata.hh patternmimedata.cc
  annotation.hh annotation.cc
  errorstack.hh errorstack.cc
  modelrom.hh modelrom.cc
  modeldefinition.hh modeldefinition.cc
  modelparser.hh modelparser.cc
  deviceclassplugininterface.hh deviceclassplugininterface.cc
  genericdevice.hh genericdevice.cc
  )

set_target_properties(libanytone-emu PROPERTIES
	MACOSX_RPATH TRUE
  OUTPUT_NAME anytone-emu
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}")
target_link_libraries(libanytone-emu PRIVATE Qt6::Core Qt6::SerialPort)

install(TARGETS libanytone-emu DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${libanytone-emu_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)
install(FILES ${PROJECT_BINARY_DIR}/lib/config.hh DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)


