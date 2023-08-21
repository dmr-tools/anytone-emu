SET(anytone-emu-gui_SOURCES
  main.cc
  setupdialog.cc setupdialog.ui
  application.cc
  imagecollectionadapter.cc imagecollectionwrapper.cc
  heximagedocument.cc heximagedumpdocument.cc hexelementdumpdocument.cc heximagediffdocument.cc
  mainwindow.cc mainwindow.ui
)
SET(anytone-emu-gui_HEADERS
  setupdialog.hh
  application.hh
  imagecollectionadapter.hh imagecollectionwrapper.hh
  heximagedocument.hh heximagedumpdocument.hh hexelementdumpdocument.hh heximagediffdocument.hh
  mainwindow.hh)


qt_add_executable(anytone-emu-gui WIN32
  ${anytone-emu-gui_SOURCES})

qt_add_resources(anytone-emu-gui "annotation"
  PREFIX "annotation"
  BASE "annotation"
  "dmr6x2uv/codeplug.xml")

target_link_libraries(anytone-emu-gui PRIVATE Qt6::Widgets Qt6::SerialPort libanytone-emu)
target_include_directories(anytone-emu-gui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Install binary only if not a bundle under MacOS X
install(TARGETS anytone-emu-gui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

