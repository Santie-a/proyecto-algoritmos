cmake_minimum_required(VERSION 3.5)
project(Algoritmos)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set macOS bundle properties
set(CMAKE_MACOSX_BUNDLE TRUE)
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.algoritmos")
set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia MultimediaWidgets Widgets)

# Find OpenCV (assuming installed via Homebrew)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add sources and headers
set(SOURCES
        main.cpp
        mainwindow.cpp
)

set(HEADERS
        mainwindow.h
)

# Add executable
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS})

# Link Qt6 and OpenCV libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::Widgets
        ${OpenCV_LIBS}
)