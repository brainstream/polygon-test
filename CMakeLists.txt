# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any means.

cmake_minimum_required(VERSION 3.28)

project(polygon_test)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PT_USE_ASAN "Eanble ASAN" OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CLIPPER2_EXAMPLES OFF)
set(CLIPPER2_TESTS OFF)
add_subdirectory(third_party/Clipper2/CPP)

set(SRC_DIR src)

set(SRC_MOC
    ${SRC_DIR}/Canvas.h
    ${SRC_DIR}/MainWindow.h
    ${SRC_DIR}/PreferencesWidget.h
    ${SRC_DIR}/Preferences.h
)

set(SRC_UI
    ${SRC_DIR}/MainWindow.ui
    ${SRC_DIR}/PreferencesWidget.ui
)

qt6_wrap_cpp(SRC_MOC ${SRC_MOC})
qt6_wrap_ui(SRC_UI ${SRC_UI})

set(SRC
    ${SRC_MOC}
    ${SRC_UI}
    ${SRC_DIR}/Canvas.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/MainWindow.cpp
    ${SRC_DIR}/Polygon.cpp
    ${SRC_DIR}/Polygon.h
    ${SRC_DIR}/PreferencesWidget.cpp
    ${SRC_DIR}/Preferences.cpp
    ${SRC_DIR}/AABB.h
)

add_executable(${PROJECT_NAME} ${SRC})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    if(PT_USE_ASAN)
        target_link_options(${PROJECT_NAME} PRIVATE /fsanitize=address)
    endif()
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    if(PT_USE_ASAN)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
    endif()
endif()

add_library(libheatmap OBJECT
    third_party/libheatmap/libheatmap/heatmap.c
    third_party/libheatmap/libheatmap/heatmap.h
    third_party/libheatmap/libheatmap/colorschemes/PuBuGn.c
    third_party/libheatmap/libheatmap/colorschemes/PuBuGn.h
)
set_target_properties(libheatmap
    PROPERTIES
    FOLDER Libraries
)
target_include_directories(libheatmap
    PRIVATE third_party/libheatmap/libheatmap
)

target_link_libraries(
    ${PROJECT_NAME}
    libheatmap
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Clipper2
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE third_party/earcut.hpp/include
    PRIVATE third_party/Clipper2/CPP/Clipper2Lib/include
    PRIVATE third_party/libheatmap
    PRIVATE ${CMAKE_BINARY_DIR}
)

add_custom_target(misc SOURCES
    README.md
)
