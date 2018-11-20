cmake_minimum_required(VERSION 2.8)

PROJECT(PDBconverter)

Find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(PDBconverter PDBconverter.cxx )

target_link_libraries(PDBconverter ${VTK_LIBRARIES})
