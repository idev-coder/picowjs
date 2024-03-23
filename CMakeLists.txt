cmake_minimum_required(VERSION 3.5)

# default target: raspberry_pi
if(NOT TARGET)
  set(TARGET "raspberry_pi")
endif()

if(NOT "${TARGET}" STREQUAL "raspberry_pi")
  project(PICOWJS_CMAKE NONE)
endif()

if(NOT VER)
  set(VER "1.0.0")
endif()

include(${CMAKE_SOURCE_DIR}/targets/${TARGET}/target.cmake)
configure_file(${CMAKE_SOURCE_DIR}/tools/picowjs_config.h.in ${CMAKE_SOURCE_DIR}/src/gen/picowjs_config.h)
