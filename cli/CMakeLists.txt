set(anytone-emu_SOURCES main.cc
	)
set(anytone-emu_HEADERS
	)

add_executable(anytone-emu WIN32 ${anytone-emu_SOURCES})
target_link_libraries(anytone-emu PRIVATE Qt6::Core Qt6::SerialPort libanytone-emu)

install(TARGETS anytone-emu DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

qt_add_resources(anytone-emu "codeplugs"
  PREFIX "/codeplugs"
  BASE "${CMAKE_SOURCE_DIR}/codeplugs/"
  FILES "${CMAKE_SOURCE_DIR}/codeplugs/catalog.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/dmr6x2uv/2.10/codeplug.xml")
