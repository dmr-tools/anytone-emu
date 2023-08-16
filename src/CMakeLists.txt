SET(anytone-emu-gui_SOURCES main.cc
)
SET(anytone-emu-gui_MOC_HEADERS
  )
SET(anytone-emu-gui_HEADERS )

qt5_wrap_cpp(anytone-emu-gui_MOC_SOURCES ${anytone-emu-gui_MOC_HEADERS})

add_executable(anytone-emu-gui WIN32
  ${anytone-emu-gui_SOURCES} ${anytone-emu-gui_MOC_SOURCES} ${anytone-emu-gui_RCC_SOURCES} ${anytone-emu-gui_UI_SOURCES})

target_link_libraries(anytone-emu-gui ${LIBS} libanytone-emu)
target_include_directories(anytone-emu-gui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Install binary only if not a bundle under MacOS X
install(TARGETS anytone-emu-gui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

