cmake_minimum_required(VERSION 3.21)
project(WP_Manager)

set(CMAKE_CXX_STANDARD 20)
set(wxWidgets_ROOT_DIR C:/wxWidgets)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

add_executable(WP_Manager WIN32 main.cpp FileUtils.h ImagesList.h WallpapersFrame.h)

target_link_libraries(WP_Manager ${wxWidgets_LIBRARIES})
