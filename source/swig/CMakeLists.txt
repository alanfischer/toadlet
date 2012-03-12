cmake_minimum_required(VERSION 2.8)
project (pytoadlet CXX C)

# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${pytoadlet_SOURCE_DIR}/../../cmake/Modules/")

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

find_package(toadlet)
include_directories(${TOADLET_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_SWIG_FLAGS "")

set_source_files_properties(toadlet/toadlet.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(toadlet/toadlet.i PROPERTIES SWIG_FLAGS "-includeall")
swig_add_module(pytoadlet python toadlet/toadlet.i)
swig_link_libraries(pytoadlet ${PYTHON_LIBRARIES} ${TOADLET_LIBRARIES})
