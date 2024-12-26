cmake_minimum_required(VERSION 3.28)

project(polygon_test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SRC_DIR src)

set(SRC
    ${SRC_DIR}/Canvas.cpp
    ${SRC_DIR}/Canvas.h
    ${SRC_DIR}/Line.cpp
    ${SRC_DIR}/Line.h
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/MainWindow.cpp
    ${SRC_DIR}/MainWindow.h
    ${SRC_DIR}/MainWindow.ui
    ${SRC_DIR}/Polygon.cpp
    ${SRC_DIR}/Polygon.h
    ${SRC_DIR}/Triangulation.cpp
    ${SRC_DIR}/Triangulation.h
    ${SRC_DIR}/VertexDirection.h
)

qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

target_include_directories(
    ${PROJECT_NAME}    
    PRIVATE third_party/mapbox
)

qt_finalize_executable(${PROJECT_NAME})
