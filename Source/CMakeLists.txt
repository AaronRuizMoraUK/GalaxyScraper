cmake_minimum_required(VERSION 3.28)

file(GLOB_RECURSE GALAXY_SCRAPER_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/Source/*.*")

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES 
    ${GALAXY_SCRAPER_SOURCE_FILES})

add_executable(GalaxyScraper
    ${GALAXY_SCRAPER_SOURCE_FILES})

# Includes
target_include_directories(GalaxyScraper PRIVATE "${CMAKE_SOURCE_DIR}/Source")

# Libraries
target_link_libraries(GalaxyScraper PRIVATE d3d9.lib)
target_link_libraries(GalaxyScraper PRIVATE Microsoft::D3DX9)
target_link_libraries(GalaxyScraper PRIVATE xinput.lib)
target_link_libraries(GalaxyScraper PRIVATE dinput8.lib)
target_link_libraries(GalaxyScraper PRIVATE dxguid.lib)
target_link_libraries(GalaxyScraper PRIVATE Winmm.lib)
target_link_libraries(GalaxyScraper PRIVATE Cal3D)
target_link_libraries(GalaxyScraper PRIVATE solid3)
target_link_libraries(GalaxyScraper PRIVATE BassShared)
target_link_libraries(GalaxyScraper PRIVATE papi)
target_link_libraries(GalaxyScraper PRIVATE lua)
target_link_libraries(GalaxyScraper PRIVATE expat)

# Post Build commands to copy DLLs to GalaxyScraper path
add_custom_command(TARGET GalaxyScraper POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:BassShared> $<TARGET_FILE_DIR:GalaxyScraper>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Microsoft::D3DX9> $<TARGET_FILE_DIR:GalaxyScraper>)

# Set the working directory for the GalaxyScraper target where it will find the data folder
set_property(TARGET GalaxyScraper PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
