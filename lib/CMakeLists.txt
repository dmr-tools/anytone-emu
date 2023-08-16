if (UNIX)
  SET(PTY_SOURCE pseudoterminalposix.cc)
  SET(PTY_HEADER pseudoterminalposix.hh)
elseif(WIN32)
  SET(PTY_SOURCE pseudoterminaldummy.cc)
  SET(PTY_HEADER pseudoterminaldummy.hh)
endif(UNIX)

SET(libanytone-emu_SOURCES
    request.cc response.cc
    device.cc ${PTY_SOURCE} logger.cc
    d868uv.cc d868uve.cc dmr6x2uv2.cc)
SET(libanytone-emu_MOC_HEADERS
    device.hh ${PTY_HEADER} logger.hh
    d868uv.hh d868uve.hh dmr6x2uv2.hh)
SET(libanytone-emu_HEADERS
    request.hh response.hh)


configure_file(config.hh.in ${PROJECT_BINARY_DIR}/lib/config.hh)

#SET(CMAKE_AUTOMOC ON)
qt5_wrap_cpp(libanytone-emu_MOC_SOURCES ${libanytone-emu_MOC_HEADERS})

add_library(libanytone-emu SHARED ${libanytone-emu_SOURCES} ${libanytone-emu_MOC_SOURCES})
set_target_properties(libanytone-emu PROPERTIES
	MACOSX_RPATH TRUE
  OUTPUT_NAME dmrconf
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}")
target_link_libraries(libanytone-emu ${CORE_LIBS})

install(TARGETS libanytone-emu DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${libanytone-emu_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)
install(FILES ${libanytone-emu_MOC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)
install(FILES ${PROJECT_BINARY_DIR}/lib/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libanytone-emu)

