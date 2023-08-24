enable_testing(true)

qt_add_executable(pattern_parser_test pattern_parser_test.cc)
add_test(NAME pattern_parser_test COMMAND pattern_parser_test)
target_link_libraries(pattern_parser_test PRIVATE Qt::Test libanytone-emu)

qt_add_executable(annotation_test annotation_test.cc)
add_test(NAME annotation_test COMMAND annotation_test)
target_link_libraries(annotation_test PRIVATE Qt::Test libanytone-emu)
