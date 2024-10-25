SET(anytone-emu-gui_SOURCES
  main.cc
  setupdialog.cc setupdialog.ui
  application.cc
  patternwrapper.cc patternview.cc patternmetaeditor.cc patternmetaeditor.ui
  codeplugdialog.cc codeplugdialog.ui
  sparserepeatdialog.cc sparserepeatdialog.ui
  blockrepeatdialog.cc blockrepeatdialog.ui
  fixedrepeatdialog.cc fixedrepeatdialog.ui
  elementdialog.cc elementdialog.ui
  integerfielddialog.cc integerfielddialog.ui
  enumfielddialog.cc enumfielddialog.ui enumfieldpatternwrapper.cc
  stringfielddialog.cc stringfielddialog.ui
  unusedfielddialog.cc unusedfielddialog.ui
  unknownpatterndialog.cc unknownpatterndialog.ui
  imagecollectionadapter.cc imagecollectionwrapper.cc
  heximagedocument.cc heximagedumpdocument.cc hexelementdumpdocument.cc heximagediffdocument.cc
  logmessagelist.cc
  mainwindow.cc mainwindow.ui
  aboutdialog.cc aboutdialog.ui
  patternselectionwidget.cc patternselectionwidget.ui
  newpatterndialog.cc newpatterndialog.ui
  splitfieldpatterndialog.cc splitfieldpatterndialog.ui
  splitfieldpatternwizard.cc splitfieldpatternwizard.ui
)

SET(anytone-emu-gui_HEADERS
  setupdialog.hh
  application.hh
  patternwrapper.hh patternview.hh patternmetaeditor.hh
  codeplugdialog.hh
  sparserepeatdialog.hh
  blockrepeatdialog.hh
  fixedrepeatdialog.hh
  elementdialog.hh
  integerfielddialog.hh
  enumfielddialog.hh enumfieldpatternwrapper.hh
  stringfielddialog.hh
  unusedfielddialog.hh
  unknownpatterndialog.hh
  imagecollectionadapter.hh imagecollectionwrapper.hh
  heximagedocument.hh heximagedumpdocument.hh hexelementdumpdocument.hh heximagediffdocument.hh
  logmessagelist.hh
  mainwindow.hh
  aboutdialog.hh
  patternselectionwidget.hh
  newpatterndialog.hh
  splitfieldpatterndialog.hh
  splitfieldpatternwizard.hh)


qt_add_executable(anytone-emu-gui WIN32
  ${anytone-emu-gui_SOURCES}
  newpatterndialog.hh newpatterndialog.cc newpatterndialog.ui
  splitfieldpatterndialog.hh splitfieldpatterndialog.cc splitfieldpatterndialog.ui
  codeplugdialog.hh codeplugdialog.cc codeplugdialog.ui
  unknownpatterndialog.hh unknownpatterndialog.cc unknownpatterndialog.ui
  elementpatternview.hh elementpatternview.cc
  elementpatterneditor.hh elementpatterneditor.cc elementpatterneditor.ui
  questiondialog.cc questiondialog.hh
  imagewidget.hh imagewidget.cc imagewidget.ui
  patternwidget.hh patternwidget.cc patternwidget.ui)

qt_add_resources(anytone-emu-gui "codeplugs"
  PREFIX "/codeplugs"
  BASE "${CMAKE_SOURCE_DIR}/codeplugs/"
  FILES "${CMAKE_SOURCE_DIR}/codeplugs/catalog.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/d868uve/2.40/codeplug.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/d878uv/3.04/codeplug.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/d878uv2/3.04/codeplug.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/d578uv/1.21/codeplug.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/d578uv2/2.03/codeplug.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/d578uv2/2.08/codeplug.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/dmr6x2uv/2.10/codeplug.xml"
        "${CMAKE_SOURCE_DIR}/codeplugs/dmr6x2uv2/1.10/codeplug.xml")


generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS pattern-codeplug pattern-codeplug-okay pattern-codeplug-warning pattern-codeplug-critical
        pattern-sparserepeat pattern-sparserepeat-okay pattern-sparserepeat-warning pattern-sparserepeat-critical
        pattern-blockrepeat pattern-blockrepeat-okay pattern-blockrepeat-warning pattern-blockrepeat-critical
        pattern-fixedrepeat pattern-fixedrepeat-okay pattern-fixedrepeat-warning pattern-fixedrepeat-critical
        pattern-element pattern-element-okay pattern-element-warning pattern-element-critical
        pattern-integer pattern-integer-okay pattern-integer-warning pattern-integer-critical
        pattern-enum pattern-enum-okay pattern-enum-warning pattern-enum-critical
        pattern-stringfield pattern-stringfield-okay pattern-stringfield-warning pattern-stringfield-critical
        pattern-unused pattern-unused-okay pattern-unused-warning pattern-unused-critical
        pattern-unknown pattern-unknown-okay pattern-unknown-warning pattern-unknown-critical
  THEMES light dark
  CONTEXT pattern
  SIZES 16 32 64)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS settings-interface camera-photo
  THEMES light dark
  CONTEXT categories
  SIZES 16 48 64)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS application-anytone-emu
  THEMES light dark
  CONTEXT apps
  SIZES 16 48 64)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS application-exit dialog-ok dialog-cancel document-new document-save document-save-as
        edit edit-annotate edit-clear edit-delete edit-erase edit-split-element edit-copy edit-paste
        insert-child insert-above insert-below
        none show-diff-single show-diff-multiple show-hexdump inspect
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

