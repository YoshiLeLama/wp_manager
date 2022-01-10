cmake_minimum_required(VERSION 3.21)
project(WP_Manager)

set(CMAKE_CXX_STANDARD 20)
set(wxWidgets_ROOT_DIR C:/wxWidgets)
set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

add_executable(WP_Manager WIN32 main.cpp FileUtils.h ImagesList.h WallpapersFrame.h ${APP_ICON_RESOURCE_WINDOWS} ColorSettings.cpp ColorSettings.h ImagesList.cpp FileUtils.cpp WallpapersFrame.cpp)

target_link_libraries(WP_Manager ${wxWidgets_LIBRARIES})
