# C++ Course at HSE by S.Shershakov
# Some stuff here is inspired and provided by the checking framework at SHAD and its followers
# leading HSE C++ Courses. Many thanks to all contributors!

cmake_minimum_required(VERSION 3.5)

project(cpp-course-stu-prj)

# configuring CMake project
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# import catch2 library either as a fullfledjed CMake-project or its simplified amalgamized version
include(cmake/EnableCatch2.cmake)

#prepare options for proper building
include(cmake/BuildFlags.cmake)

### Individual tasks are grouped in a separated directory
add_subdirectory(tasks)
