cmake_minimum_required(VERSION 3.2)

project(multi2sim)

add_definitions(-DVERSION="5.0")


include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}")


set(SUBDIRECTORIES src)
set (CMAKE_CXX_STANDARD 11)


foreach( subdir ${SUBDIRECTORIES} )
    add_subdirectory( ${subdir} )
endforeach( subdir )
