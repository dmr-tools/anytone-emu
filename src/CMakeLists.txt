SET(anytone-emu-gui_SOURCES
  main.cc
  setupdialog.cc setupdialog.ui
  application.cc
  patternwrapper.cc patternview.cc patternmetaeditor.cc patternmetaeditor.ui
  sparserepeatdialog.cc sparserepeatdialog.ui
  blockrepeatdialog.cc blockrepeatdialog.ui
  elementdialog.cc elementdialog.ui
  integerfielddialog.cc integerfielddialog.ui
  enumfielddialog.cc enumfielddialog.ui enumfieldpatternwrapper.cc
  stringfielddialog.cc stringfielddialog.ui
  unusedfielddialog.cc unusedfielddialog.ui
  imagecollectionadapter.cc imagecollectionwrapper.cc
  heximagedocument.cc heximagedumpdocument.cc hexelementdumpdocument.cc heximagediffdocument.cc
  logmessagelist.cc
  mainwindow.cc mainwindow.ui
)

SET(anytone-emu-gui_HEADERS
  setupdialog.hh
  application.hh
  patternwrapper.hh patternview.hh patternmetaeditor.hh
  sparserepeatdialog.hh
  blockrepeatdialog.hh
  elementdialog.hh
  integerfielddialog.hh
  enumfielddialog.hh enumfieldpatternwrapper.hh
  stringfielddialog.hh
  unusedfielddialog.hh
  imagecollectionadapter.hh imagecollectionwrapper.hh
  heximagedocument.hh heximagedumpdocument.hh hexelementdumpdocument.hh heximagediffdocument.hh
  mainwindow.hh
  logmessagelist.hh)


qt_add_executable(anytone-emu-gui WIN32
  ${anytone-emu-gui_SOURCES})

qt_add_resources(anytone-emu-gui "codeplugs"
  PREFIX "/codeplugs"
  BASE "${CMAKE_SOURCE_DIR}/codeplugs/"
  FILES "${CMAKE_SOURCE_DIR}/codeplugs/dmr6x2uv/codeplug.xml")

generate_icons(ICONS
  "${CMAKE_SOURCE_DIR}/shared/icons/codeplugpattern.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/sparserepeat.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/blockrepeat.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/fixedrepeat.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/element.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/integer.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/enum.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/stringfield.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/unused.svg"
  "${CMAKE_SOURCE_DIR}/shared/icons/unknown.svg"
  SIZES 16 32 64)
qt_add_resources(anytone-emu-gui "icons"
  PREFIX "icons/"
  BASE "${CMAKE_CURRENT_BINARY_DIR}/icons/"
  FILES "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/codeplugpattern.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/codeplugpattern.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/sparserepeat.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/sparserepeat.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/blockrepeat.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/blockrepeat.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/fixedrepeat.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/fixedrepeat.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/element.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/element.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/integer.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/integer.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/enum.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/enum.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/stringfield.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/stringfield.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/unused.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/unused.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/64x64/unknown.png"
        "${CMAKE_CURRENT_BINARY_DIR}/icons/16x16/unknown.png")

target_link_libraries(anytone-emu-gui PRIVATE Qt6::Widgets Qt6::SerialPort libanytone-emu)
target_include_directories(anytone-emu-gui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Install binary only if not a bundle under MacOS X
install(TARGETS anytone-emu-gui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

