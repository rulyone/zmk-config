# Runs because module.yml points here.
zephyr_library()

# Helpful debug so you can see what's happening in CI logs:
message(STATUS "zmk_user_config: SHIELD_AS_LIST=${SHIELD_AS_LIST}")

# Module dir is <repo>/zephyr; compute repo root:
get_filename_component(REPO_ROOT "${ZEPHYR_CURRENT_MODULE_DIR}" DIRECTORY)

# Add the source only when corne_left is selected
if("corne_left" IN_LIST SHIELD_AS_LIST)
  zephyr_library_sources(
    ${REPO_ROOT}/boards/shields/corne_left/src/layer_underglow.c
  )
endif()
