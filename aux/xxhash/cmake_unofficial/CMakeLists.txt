cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)

project(xxhash)

set(XXHASH_LIB_VERSION "0.42.0")
set(XXHASH_LIB_SOVERSION "0")

option(BUILD_XXHSUM "Build the xxhsum binary" ON)

# Make CMake's RPATH handling not be insane. This suff has cmake set rpaths
# appropriately for where things end up in the install tree. For some reason
# that's not the default: https://cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# Where we search for shared libraries
set(CMAKE_INSTALL_RPATH "./lib")

# add the automatically determined parts of the RPATH which point to directories
# outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(xxhash ../xxhash.c)
set_target_properties(
  xxhash
  PROPERTIES COMPILE_DEFINITIONS "XXHASH_EXPORT"
             VERSION "${XXHASH_LIB_VERSION}" SOVERSION
                                             "${XXHASH_LIB_SOVERSION}")

if (BUILD_XXHSUM)
  add_executable(xxhsum ../xxhsum.c)
  target_link_libraries(xxhsum xxhash)
endif ()

install(FILES ../xxhash.h DESTINATION include)
install(
  TARGETS xxhash xxhsum
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
