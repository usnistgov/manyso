cmake_minimum_required(VERSION 2.8)
project(manyso)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/REFPROP-headers")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(so SHARED "${CMAKE_CURRENT_SOURCE_DIR}/sample_so/so.cpp")

add_executable(simple_test "${CMAKE_CURRENT_SOURCE_DIR}/tests/simple_tests.cpp")
add_executable(autowrap_test "${CMAKE_CURRENT_SOURCE_DIR}/tests/autowrap_tests.cpp")
add_executable(refpropwrap_test "${CMAKE_CURRENT_SOURCE_DIR}/tests/refpropwrap_tests.cpp")

if (WIN32)
  set(TESTSUITE TRUE CACHE BOOL "Compile MemoryModule with TESTSUITE support")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/MemoryModule")
  target_link_libraries(simple_test MemoryModule)
  target_link_libraries(autowrap_test MemoryModule)
  target_link_libraries(refpropwrap_test MemoryModule)
else()
  target_link_libraries(refpropwrap_test ${CMAKE_DL_LIBS})
endif()
