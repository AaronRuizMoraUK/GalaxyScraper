cmake_minimum_required(VERSION 3.28)

include(FetchContent)

set(3rdparty_folder "3rdParty")

# ---------------------------------------------------------------------
# DirectXTex
#
# SLibrary for reading and writing .DDS files.
# ---------------------------------------------------------------------

set(BUILD_TOOLS OFF CACHE INTERNAL "Build tex command-line tools")
set(BUILD_SAMPLE OFF CACHE INTERNAL "Build DDSView sample (requires fxc.exe)")
set(BUILD_TESTING OFF CACHE INTERNAL "Build tests")
set(BUILD_DX11 OFF CACHE INTERNAL "Build with DirectX11 Runtime support (requires fxc.exe)")
set(BUILD_DX12 OFF CACHE INTERNAL "Build with DirectX12 Runtime support")
set(BC_USE_OPENMP OFF CACHE INTERNAL "Build with OpenMP support")
set(ENABLE_LIBJPEG_SUPPORT OFF CACHE INTERNAL "Build with libjpeg support")
set(ENABLE_LIBPNG_SUPPORT OFF CACHE INTERNAL "Build with libpng support")

FetchContent_Declare(
    DirectXTex
    GIT_REPOSITORY https://github.com/microsoft/DirectXTex.git
    GIT_TAG jun2024
)

FetchContent_MakeAvailable(DirectXTex)

file(GLOB_RECURSE DDS_SOURCE_FILES
    "${DirectXTex_SOURCE_DIR}/DDSTextureLoader/DDSTextureLoader9.*")
source_group(TREE "${DirectXTex_SOURCE_DIR}/DDSTextureLoader" FILES ${DDS_SOURCE_FILES})

add_library(DDSTextureLoader ${DDS_SOURCE_FILES})
target_include_directories(DDSTextureLoader PUBLIC "${DirectXTex_SOURCE_DIR}/DDSTextureLoader") 

set(DirectXTex_folder "${3rdparty_folder}/DirectXTex")
set_target_properties(DirectXTex PROPERTIES FOLDER ${DirectXTex_folder})
set_target_properties(DDSTextureLoader PROPERTIES FOLDER ${DirectXTex_folder})
