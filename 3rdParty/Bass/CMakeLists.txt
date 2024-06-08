cmake_minimum_required(VERSION 3.28)

set(bass_folder "3rdParty/Bass")
set(bass_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${bass_folder}")

file(GLOB_RECURSE BASS_SOURCE_FILES
    "${bass_SOURCE_DIR}/c/*.*")

source_group(TREE "${bass_SOURCE_DIR}/c" FILES 
    ${BASS_SOURCE_FILES})

add_library(Bass INTERFACE ${BASS_SOURCE_FILES})

target_include_directories(Bass INTERFACE "${bass_SOURCE_DIR}/c")
target_link_directories(Bass INTERFACE "${bass_SOURCE_DIR}/x64")

target_link_libraries(Bass INTERFACE bass)

set_target_properties(Bass PROPERTIES FOLDER ${bass_folder})
