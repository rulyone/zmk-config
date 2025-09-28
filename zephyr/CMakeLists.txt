zephyr_library()

# Debug what the build sees (will print in CMake configure logs)
message(STATUS "zmk_user_config: CONFIG_SHIELD_CORNE_LEFT=${CONFIG_SHIELD_CORNE_LEFT}")
message(STATUS "zmk_user_config: CONFIG_ZMK_RGB_UNDERGLOW=${CONFIG_ZMK_RGB_UNDERGLOW}")

# Module dir is <repo>/zephyr; go up to repo root:
get_filename_component(REPO_ROOT "${ZEPHYR_CURRENT_MODULE_DIR}" DIRECTORY)

if(CONFIG_SHIELD_CORNE_LEFT)
  zephyr_library_sources(
    ${REPO_ROOT}/config/boards/shields/corne_left/src/layer_underglow.c
  )
endif()
