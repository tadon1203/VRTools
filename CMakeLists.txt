cmake_minimum_required(VERSION 4.1)
project(VRTools)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        minhook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG v1.3.4
)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.5
)
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-14-1
)
FetchContent_MakeAvailable(minhook fmt nlohmann_json imgui freetype)

file(GLOB_RECURSE SOURCES_LIST CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")

add_library(VRTools SHARED
        ${SOURCES_LIST}
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
        ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)

target_include_directories(VRTools PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/extern
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${freetype_INCLUDE_DIRS}
)

target_compile_definitions(VRTools PRIVATE IMGUI_ENABLE_FREETYPE _USE_MATH_DEFINES)

target_link_libraries(VRTools PRIVATE minhook fmt::fmt nlohmann_json::nlohmann_json d3d11 freetype)