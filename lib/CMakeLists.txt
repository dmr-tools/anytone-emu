if (UNIX)
  SET(PTY_SOURCE pseudoterminalposix.cc)
  SET(PTY_HEADER pseudoterminalposix.hh)
elseif(WIN32)
  SET(PTY_SOURCE pseudoterminalwindows.cc)
  SET(PTY_HEADER pseudoterminalwindows.hh)
endif(UNIX)

SET(libanytone-emu_SOURCES
    request.cc response.cc hexdump.cc
    device.cc ${PTY_SOURCE} logger.cc model.cc image.cc
    d868uv.cc d868uve.cc d878uv.cc d878uv2.cc d578uv.cc d578uv2.cc
    dmr6x2uv.cc dmr6x2uv2.cc
    djmd5.cc djmd5x.cc)
SET(libanytone-emu_MOC_HEADERS
    device.hh ${PTY_HEADER} logger.hh model.hh image.hh
    d868uv.hh d868uve.hh d878uv.hh d878uv2.hh d578uv.hh d578uv2.hh
    dmr6x2uv.hh dmr6x2uv2.hh
    djmd5.hh djmd5x.hh)
SET(libanytone-emu_HEADERS pseudoterminal.hh
    request.hh response.hh hexdump.hh)


configure_file(config.hh.in ${PROJECT_BINARY_DIR}/lib/config.hh)

#SET(CMAKE_AUTOMOC ON)
qt5_wrap_cpp(libanytone-emu_MOC_SOURCES ${libanytone-emu_MOC_HEADERS})

add_library(libanytone-emu STATIC ${libanytone-emu_SOURCES} ${libanytone-emu_MOC_SOURCES})
set_target_properties(libanytone-emu PROPERTIES
	MACOSX_RPATH TRUE
  OUTPUT_NAME anytone-emu
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}")
target_link_libraries(libanytone-emu ${CORE_LIBS})

install(TARGETS libanytone-emu DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${libanytone-emu_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)
install(FILES ${libanytone-emu_MOC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)
install(FILES ${PROJECT_BINARY_DIR}/lib/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)

