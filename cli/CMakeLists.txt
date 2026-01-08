
add_executable(anytone-emu WIN32
  main.cc)
target_link_libraries(anytone-emu PRIVATE Qt6::Core Qt6::SerialPort libanytone-emu
        anytone-emu-anytone anytone-emu-opengd77 anytone-emu-radtel anytone-emu-md32uv)

install(TARGETS anytone-emu DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
