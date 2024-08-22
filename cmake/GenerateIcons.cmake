set(GENERATE_ICONS_OUTPUT_FILES "")

function(_create_build_icon_command icon_name icon_source_dir icon_theme icon_context icon_size icon_output_dir)
  set(input_file "${icon_source_dir}/${icon_theme}/${icon_context}/${icon_name}.svg")
  set(output_file "${icon_output_dir}/${icon_theme}/${icon_size}x${icon_size}/${icon_context}/${icon_name}.png")
  list(APPEND GENERATE_ICONS_OUTPUT_FILES ${output_file})
  add_custom_command(
    OUTPUT ${output_file}
    COMMAND convert ARGS -background none -resize ${icon_size}x${icon_size} ${input_file} ${output_file}
    DEPENDS ${input_file}
    COMMENT "Generate ${output_file}"
    VERBATIM)
  return(PROPAGATE GENERATE_ICONS_OUTPUT_FILES)
endfunction()

function(generate_icons)
  cmake_parse_arguments(GENERATE_ICONS "" "DIRECTORY;CONTEXT;" "THEMES;ICONS;SIZES" ${ARGN})

  set(GENERATE_ICONS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/icons/")

  foreach(ICON_THEME ${GENERATE_ICONS_THEMES})
    file(MAKE_DIRECTORY "${GENERATE_ICONS_OUTPUT_DIR}/${ICON_THEME}/scalable/${GENERATE_ICONS_CONTEXT}")
    foreach(ICON_NAME ${GENERATE_ICONS_ICONS})
      set(source_file "${GENERATE_ICONS_DIRECTORY}/${ICON_THEME}/${GENERATE_ICONS_CONTEXT}/${ICON_NAME}.svg")
      set(output_file "${GENERATE_ICONS_OUTPUT_DIR}/${ICON_THEME}/scalable/${GENERATE_ICONS_CONTEXT}/${ICON_NAME}.svg")
      list(APPEND GENERATE_ICONS_OUTPUT_FILES ${output_file})
      add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${CMAKE_COMMAND} -E copy ${source_file} "${GENERATE_ICONS_OUTPUT_DIR}/${ICON_THEME}/scalable/${GENERATE_ICONS_CONTEXT}"
        DEPENDS ${source_file}
        COMMENT "Copy ${output_file}"
        VERBATIM)
    endforeach()

    foreach(ICON_SIZE ${GENERATE_ICONS_SIZES})
      file(MAKE_DIRECTORY "${GENERATE_ICONS_OUTPUT_DIR}/${ICON_THEME}/${ICON_SIZE}x${ICON_SIZE}/${GENERATE_ICONS_CONTEXT}")
      foreach(ICON_NAME ${GENERATE_ICONS_ICONS})
        set(source_file "${GENERATE_ICONS_DIRECTORY}/${ICON_THEME}/${GENERATE_ICONS_CONTEXT}/${ICON_NAME}.svg")
        _create_build_icon_command(${ICON_NAME} ${GENERATE_ICONS_DIRECTORY} ${ICON_THEME} ${GENERATE_ICONS_CONTEXT} ${ICON_SIZE} ${GENERATE_ICONS_OUTPUT_DIR})
      endforeach()
    endforeach()
  endforeach()
  return(PROPAGATE GENERATE_ICONS_OUTPUT_FILES)
endfunction()

