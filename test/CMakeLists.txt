enable_testing(true)

qt_add_executable(address_test address_test.cc)
add_test(NAME address_test COMMAND address_test)
target_link_libraries(address_test PRIVATE Qt::Test libanytone-emu)

qt_add_executable(pattern_parser_test pattern_parser_test.cc)
add_test(NAME pattern_parser_test COMMAND pattern_parser_test)
target_link_libraries(pattern_parser_test PRIVATE Qt::Test libanytone-emu)

qt_add_executable(annotation_test annotation_test.cc)
add_test(NAME annotation_test COMMAND annotation_test)
target_link_libraries(annotation_test PRIVATE Qt::Test libanytone-emu)

qt_add_executable(model_parser_test modelparsertest.cc)
add_test(NAME model_parser_test COMMAND model_parser_test)
target_link_libraries(model_parser_test PRIVATE Qt::Test libanytone-emu)
