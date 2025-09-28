# Runs because module.yml points to this directory.
zephyr_library()

# Make ZMK public headers visible to this module's sources
# PROJECT_SOURCE_DIR == <zmk repo>/zmk/app when building zmk/app as the sample
zephyr_library_include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/module/include
)

# Debug so we can see it in CI logs:
message(STATUS "zmk_user_config: CMake is running")
message(STATUS "zmk_user_config: SHIELD=${SHIELD}")
message(STATUS "zmk_user_config: SHIELD_AS_LIST=${SHIELD_AS_LIST}")
message(STATUS "zmk_user_config: ZEPHYR_CURRENT_MODULE_DIR=${ZEPHYR_CURRENT_MODULE_DIR}")

# The module dir **is** the repo root (because module.yml lives in zephyr/)
set(LAYER_FILE "${ZEPHYR_CURRENT_MODULE_DIR}/boards/shields/corne_left/src/layer_underglow.c")
if(EXISTS "${LAYER_FILE}")
  message(STATUS "zmk_user_config: found ${LAYER_FILE}")
else()
  message(WARNING "zmk_user_config: ${LAYER_FILE} not found")
endif()

# Gate on SHIELD early (works during configure time)
if( (DEFINED SHIELD AND SHIELD STREQUAL "corne_left")
    OR ("corne_left" IN_LIST SHIELD_AS_LIST) )
  message(STATUS "zmk_user_config: corne_left detected; adding layer_underglow.c")
  zephyr_library_sources("${LAYER_FILE}")
endif()
