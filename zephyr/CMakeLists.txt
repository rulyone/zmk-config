zephyr_library()

# ----- Add ZMK app headers to this module's include path -----
# CMAKE_SOURCE_DIR resolves to <workspace>/zmk/app for this build.
set(ZMK_APP_INCLUDE         "${CMAKE_SOURCE_DIR}/include")
set(ZMK_APP_MODULE_INCLUDE  "${CMAKE_SOURCE_DIR}/module/include")

# Helpful diagnostics in the CMake log
if(EXISTS "${ZMK_APP_INCLUDE}/zmk/event_manager.h")
  message(STATUS "zmk_user_config: Found event_manager.h in ${ZMK_APP_INCLUDE}")
else()
  message(WARNING "zmk_user_config: event_manager.h NOT found under ${ZMK_APP_INCLUDE}")
  message(WARNING "zmk_user_config: CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
endif()

# Attach the include dirs to THIS library (created by zephyr_library())
# so the sources (e.g., layer_underglow.c) can include ZMK headers.
target_include_directories(${ZEPHYR_CURRENT_LIBRARY} PRIVATE
  ${ZMK_APP_INCLUDE}
  ${ZMK_APP_MODULE_INCLUDE}
)
# -------------------------------------------------------------
set(LAYER_FILE "${ZEPHYR_CURRENT_MODULE_DIR}/boards/shields/corne_left/src/layer_underglow.c")
if(EXISTS "${LAYER_FILE}")
  message(STATUS "zmk_user_config: found ${LAYER_FILE}")
else()
  message(WARNING "zmk_user_config: ${LAYER_FILE} not found")
endif()

# Gate on SHIELD early (works during configure time)
# with this we only apply the layer_underglow.c only to corne_left
if( (DEFINED SHIELD AND SHIELD STREQUAL "corne_left")
    OR ("corne_left" IN_LIST SHIELD_AS_LIST) )
  message(STATUS "zmk_user_config: corne_left detected; adding layer_underglow.c")
  zephyr_library_sources("${LAYER_FILE}")
endif()
