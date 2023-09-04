function(_create_build_icon_command icon_source_file icon_size)
  get_filename_component(icon_base_name ${icon_source_file} NAME_WE)
  set(input_file ${icon_source_file})
  set(output_file ${CMAKE_CURRENT_BINARY_DIR}/icons/${icon_size}x${icon_size}/${icon_base_name}.png)
  #message("Define icon generation ${input_file} -> ${output_file}")
  add_custom_command(
    OUTPUT ${output_file} ${output_file_32} ${output_file_16}
    COMMAND convert ${input_file} ARGS -resize 64x64 ${output_file}
    DEPENDS ${input_file}
    COMMENT "Generate ${output_file}"
    VERBATIM)
endfunction()

function(generate_icons)
  cmake_parse_arguments(GENERATE_ICONS "" "" "ICONS;SIZES" ${ARGN})
  #message("Generate icons from ${GENERATE_ICONS_ICONS} for sizes ${GENERATE_ICONS_SIZES}")
  foreach(ICON_SIZE ${GENERATE_ICONS_SIZES})
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/icons/${ICON_SIZE}x${ICON_SIZE}/")
    foreach(SOURCE_ICON ${GENERATE_ICONS_ICONS})
      _create_build_icon_command(${SOURCE_ICON} ${ICON_SIZE})
    endforeach()
  endforeach()
endfunction()


