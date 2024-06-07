cmake_minimum_required(VERSION 3.28)

set(expat_folder "3rdParty/expat")
set(expat_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${expat_folder}")

file(GLOB_RECURSE EXPAT_SOURCE_FILES
    "${expat_SOURCE_DIR}/Source/*.*")

source_group(TREE "${expat_SOURCE_DIR}" FILES 
    ${EXPAT_SOURCE_FILES})

add_library(expat INTERFACE ${EXPAT_SOURCE_FILES})

target_include_directories(expat INTERFACE "${expat_SOURCE_DIR}/Source/lib")
target_link_directories(expat INTERFACE "${expat_SOURCE_DIR}/Libs")

target_link_libraries(expat INTERFACE libexpat)

set_target_properties(expat PROPERTIES FOLDER ${expat_folder})
