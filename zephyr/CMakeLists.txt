# This CMake runs because module.yml points here.
# ZEPHYR_CURRENT_MODULE_DIR is the absolute path to the directory containing module.yml (i.e., your repo's ./zephyr/.. root).

zephyr_library()

# Only for corne_left builds:
if(CONFIG_SHIELD_CORNE_LEFT)
  zephyr_library_sources(
    ${ZEPHYR_CURRENT_MODULE_DIR}/config/boards/shields/corne_left/src/layer_underglow.c
  )
endif()
