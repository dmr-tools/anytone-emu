qt6_add_plugin(anytone-emu-anytone STATIC CLASS_NAME AnytoneDeviceClassPlugin)
target_sources(anytone-emu-anytone PRIVATE
  anytone/deviceclass.hh anytone/deviceclass.cc
  anytone/protocol.hh anytone/protocol.cc
  anytone/device.hh anytone/device.cc
  anytone/parser.hh anytone/parser.cc
  anytone/definition.hh anytone/definition.cc)

target_include_directories(anytone-emu-anytone PUBLIC ../lib)
target_link_libraries(anytone-emu-anytone PRIVATE Qt6::Core libanytone-emu)


qt6_add_plugin(anytone-emu-opengd77 STATIC CLASS_NAME OpenGD77DeviceClassPlugin)
target_sources(anytone-emu-opengd77 PRIVATE
  opengd77/deviceclass.hh opengd77/deviceclass.cc
  opengd77/device.hh opengd77/device.cc
  opengd77/protocol.hh opengd77/protocol.cc
  opengd77/definition.hh opengd77/definition.cc)

target_include_directories(anytone-emu-opengd77 PUBLIC ../lib)
target_link_libraries(anytone-emu-opengd77 PRIVATE Qt6::Core libanytone-emu)


qt6_add_plugin(anytone-emu-radtel STATIC CLASS_NAME RadtelDeviceClassPlugin)
target_sources(anytone-emu-radtel PRIVATE
  radtel/deviceclass.hh radtel/deviceclass.cc
  radtel/device.hh radtel/device.cc
  radtel/protocol.hh radtel/protocol.cc
  radtel/definition.hh radtel/definition.cc)

target_include_directories(anytone-emu-radtel PUBLIC ../lib)
target_link_libraries(anytone-emu-radtel PRIVATE Qt6::Core libanytone-emu)


qt6_add_plugin(anytone-emu-md32uv STATIC CLASS_NAME MD32UVDeviceClassPlugin)
target_sources(anytone-emu-md32uv PRIVATE
  md32uv/deviceclass.hh md32uv/deviceclass.cc
  md32uv/device.hh md32uv/device.cc
  md32uv/protocol.hh md32uv/protocol.cc
  md32uv/definition.hh md32uv/definition.cc)

target_include_directories(anytone-emu-md32uv PUBLIC ../lib)
target_link_libraries(anytone-emu-md32uv PRIVATE Qt6::Core libanytone-emu)
