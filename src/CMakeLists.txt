SET(anytone-emu-gui_SOURCES
  main.cc
  setupdialog.cc setupdialog.ui
  application.cc
  patternwrapper.cc patternview.cc patternmetaeditor.cc patternmetaeditor.ui
  sparserepeatdialog.cc sparserepeatdialog.ui
  blockrepeatdialog.cc blockrepeatdialog.ui
  fixedrepeatdialog.cc fixedrepeatdialog.ui
  elementdialog.cc elementdialog.ui
  integerfielddialog.cc integerfielddialog.ui
  enumfielddialog.cc enumfielddialog.ui enumfieldpatternwrapper.cc
  stringfielddialog.cc stringfielddialog.ui
  unusedfielddialog.cc unusedfielddialog.ui
  imagecollectionadapter.cc imagecollectionwrapper.cc
  heximagedocument.cc heximagedumpdocument.cc hexelementdumpdocument.cc heximagediffdocument.cc
  logmessagelist.cc
  mainwindow.cc mainwindow.ui
  aboutdialog.cc aboutdialog.ui
  patternselectionwidget.cc patternselectionwidget.ui
  newpatterndialog.cc newpatterndialog.ui
)

SET(anytone-emu-gui_HEADERS
  setupdialog.hh
  application.hh
  patternwrapper.hh patternview.hh patternmetaeditor.hh
  sparserepeatdialog.hh
  blockrepeatdialog.hh
  fixedrepeatdialog.hh
  elementdialog.hh
  integerfielddialog.hh
  enumfielddialog.hh enumfieldpatternwrapper.hh
  stringfielddialog.hh
  unusedfielddialog.hh
  imagecollectionadapter.hh imagecollectionwrapper.hh
  heximagedocument.hh heximagedumpdocument.hh hexelementdumpdocument.hh heximagediffdocument.hh
  logmessagelist.hh
  mainwindow.hh
  aboutdialog.hh
  patternselectionwidget.hh
  newpatterndialog.hh)


qt_add_executable(anytone-emu-gui WIN32
  ${anytone-emu-gui_SOURCES}
  newpatterndialog.hh newpatterndialog.cc newpatterndialog.ui)

qt_add_resources(anytone-emu-gui "codeplugs"
  PREFIX "/codeplugs"
  BASE "${CMAKE_SOURCE_DIR}/codeplugs/"
  FILES "${CMAKE_SOURCE_DIR}/codeplugs/dmr6x2uv/codeplug.xml")


generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS pattern-codeplug pattern-sparserepeat pattern-blockrepeat pattern-fixedrepeat
        pattern-element pattern-integer pattern-enum pattern-stringfield pattern-unused
        pattern-unknown
  THEMES light dark
  CONTEXT pattern
  SIZES 16 32 64)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS settings-interface
  THEMES light dark
  CONTEXT categories
  SIZES 64)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS application-anytone-emu
  THEMES light dark
  CONTEXT apps
  SIZES 16 48 64)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS application-exit dialog-ok dialog-cancel document-save document-save-as
        edit edit-annotate edit-clear edit-delete edit-erase
        help-about help-contents show-log window-close
  THEMES light dark
  CONTEXT actions
  SIZES 16)


qt_add_resources(anytone-emu-gui "icons"
  PREFIX "icons"
  BASE "${CMAKE_CURRENT_BINARY_DIR}/icons"
  FILES ${GENERATE_ICONS_OUTPUT_FILES})

qt_add_resources(anytone-emu-gui "icons-themes"
  PREFIX "icons"
  BASE "${CMAKE_SOURCE_DIR}/shared/icons"
  FILES "${CMAKE_SOURCE_DIR}/shared/icons/light/index.theme"
        "${CMAKE_SOURCE_DIR}/shared/icons/dark/index.theme")


target_link_libraries(anytone-emu-gui PRIVATE Qt6::Widgets Qt6::SerialPort libanytone-emu)
target_include_directories(anytone-emu-gui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Install binary only if not a bundle under MacOS X
install(TARGETS anytone-emu-gui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

