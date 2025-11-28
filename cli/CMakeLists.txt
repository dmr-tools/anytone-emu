
add_executable(anytone-emu WIN32
  main.cc)
target_link_libraries(anytone-emu PRIVATE Qt6::Core Qt6::SerialPort libanytone-emu)

install(TARGETS anytone-emu DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
