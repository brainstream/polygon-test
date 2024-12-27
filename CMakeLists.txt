cmake_minimum_required(VERSION 3.28)

project(polygon_test)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(third_party/Clipper2/CPP)

set(SRC_DIR src)

set(SRC_MOC
    ${SRC_DIR}/Canvas.h
    ${SRC_DIR}/MainWindow.h
)

set(SRC_UI
    ${SRC_DIR}/MainWindow.ui
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
    ${SRC_DIR}/Triangulation.cpp
    ${SRC_DIR}/Triangulation.h
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(
    ${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Clipper2
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE third_party/mapbox
    PRIVATE third_party/Clipper2/CPP/Clipper2Lib/include
    PRIVATE ${CMAKE_BINARY_DIR}
)
