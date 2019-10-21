cmake_minimum_required(VERSION 3.5)

project(NextcloudShareDownloader
        VERSION 0.9
        DESCRIPTION "Downloads / Syncs files from a Nextclound share"
        HOMEPAGE_URL "https://github.com/adrido/NextcloudShareDownloader"
        LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
set(OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSL/Win_x64/")
find_package(OpenSSL 1.1 REQUIRED )

add_executable(Nextxcloudsync WIN32
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  propfindparser.cpp
  propfindparser.h
  remotediriterator.cpp
  remotediriterator.h
  messagelogger.cpp
  messagelogger.h
  downloadwidget.cpp
  downloadwidget.h
  Resource.qrc
)

target_link_libraries(Nextxcloudsync PRIVATE
    Qt5::Widgets
    Qt5::Network)

include(DeployQt)
windeployqt(Nextxcloudsync)
file(COPY
    ${OPENSSL_ROOT_DIR}/bin/libcrypto-1_1-x64.dll
    ${OPENSSL_ROOT_DIR}/bin/libssl-1_1-x64.dll
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Deployment / Installation
############################

set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")

include(InstallRequiredSystemLibraries)
install(TARGETS Nextxcloudsync DESTINATION "./")




install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Qt5Core.dll
    ${CMAKE_CURRENT_BINARY_DIR}/Qt5Gui.dll
    ${CMAKE_CURRENT_BINARY_DIR}/Qt5Network.dll
    ${CMAKE_CURRENT_BINARY_DIR}/Qt5Widgets.dll
    ${CMAKE_CURRENT_BINARY_DIR}/Qt5Widgets.dll
    ${CMAKE_CURRENT_BINARY_DIR}/libcrypto-1_1-x64.dll
    ${CMAKE_CURRENT_BINARY_DIR}/libssl-1_1-x64.dll
    DESTINATION "./")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/platforms/ DESTINATION platforms/ )
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/styles/ DESTINATION styles/ )
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/translations/ DESTINATION translations/ )


include(CPack)

