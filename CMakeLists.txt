cmake_minimum_required ( VERSION 2.6 )
project ( PracaMagisterska )

# EXECUTABLE_OUTPUT_PATH
# LIBRARY_OUTPUT_PATH

if ( NOT CMAKE_BUILD_TYPE )
  set ( CMAKE_BUILD_TYPE "Debug" )
  set ( CMAKE_CXX_COMPILER "/usr/bin/colorgcc" )
endif ( NOT CMAKE_BUILD_TYPE )

set ( CMAKE_CXX_FLAGS_DEBUG " -ggdb -o0 -Wall -lstdc++" )
set ( CMAKE_CXX_FLAGS_RELEASE " -O2 -lstdc++" )
message ( "build type: ${CMAKE_BUILD_TYPE}." )
set ( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
set ( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib )
message ( "dir for binary: " ${EXECUTABLE_OUTPUT_PATH} "; \nlib path: " ${LIBRARY_OUTPUT_PATH} )
set ( PROJECT_INC_DIR ${PROJECT_SOURCE_DIR} )
#set ( SUBDIRECTORIES  
#      ${PROJECT_SOURCE_DIR}
#      ${PROJECT_SOURCE_DIR}/test 
#    )

enable_testing()

include(test/gtest.cmake)

include_directories ( ${PROJECT_INC_DIR} )
#foreach ( DIR ${SUBDIRECTORIES} )
#  add_subdirectory ( ${DIR} )
#endforeach ( DIR )

# Main library
set ( LIBMIXED_SOURCES
  coroutine.cc
  epoller.cc
  fiber.cc
  scheduler.cc
  worker.cc
  master.cc
  "message.cc"
  message_queue.cc
  )

add_library ( libmixed SHARED ${LIBMIXED_SOURCES} )
target_link_libraries ( libmixed pthread )

add_subdirectory ( ${PROJECT_SOURCE_DIR}/test )
