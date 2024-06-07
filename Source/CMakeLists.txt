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
target_link_libraries(GalaxyScraper PRIVATE Cal3D)
target_link_libraries(GalaxyScraper PRIVATE solid3)
target_link_libraries(GalaxyScraper PRIVATE Bass)
target_link_libraries(GalaxyScraper PRIVATE papi)
target_link_libraries(GalaxyScraper PRIVATE lua)
target_link_libraries(GalaxyScraper PRIVATE expat)
