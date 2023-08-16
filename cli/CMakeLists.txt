set(anytone-emu_SOURCES main.cc
	)
set(anytone-emu_MOC_HEADERS )
set(anytone-emu_HEADERS ${anytone-emu_MOC_HEADERS}
	)


qt5_wrap_cpp(anytone-emu_MOC_SOURCES ${anytoneemu_MOC_HEADERS})

add_executable(anytone-emu WIN32 ${anytone-emu_SOURCES} ${anytone-emu_MOC_SOURCES})
target_link_libraries(anytone-emu ${CORE_LIBS} libanytone-emu)

install(TARGETS anytone-emu DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
