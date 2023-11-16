cmake_minimum_required(VERSION 3.16)

#############################################################################################
# Project base settings
# to be declared before other steps
#############################################################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE})

# Apple related
set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)
set(MACOSX_DEPLOYMENT_TARGET 11)

project(TextEditor LANGUAGES CXX VERSION 1.0.0)

message(STATUS "Deployment will output to ${CMAKE_INSTALL_PREFIX}")

include(GNUInstallDirs)
if(APPLE)
    # create .app package
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_MACOSX_BUNDLE ON)
elseif(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON) ### win32 app (no console)

    #on windows ignore GNUInstallDirs
    set(CMAKE_INSTALL_BINDIR .)
    set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_BINDIR})
    set(CMAKE_INSTALL_DATADIR ${CMAKE_INSTALL_BINDIR})
endif()


#############################################################################################
# Qt6 COMPONENTS
#############################################################################################
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED) # sets some usefull variables (i.e: QT_VERSION_MAJOR)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network NetworkAuth)

#############################################################################################
# My binary
#############################################################################################
qt_add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    mainwindow.cpp mainwindow.h
    mdichild.cpp mdichild.h
    MANUAL_FINALIZATION
    authenticator.h authenticator.cpp
    helperstructures.h
    abstractauthenticator.h
    authenticatorgoogle.h authenticatorgoogle.cpp
)

#############################################################################################
# LIBRARIES
#############################################################################################
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::NetworkAuth
)

#############################################################################################
# Resources
#############################################################################################
set(mdi_resource_files
    "images/copy.png"
    "images/cut.png"
    "images/new.png"
    "images/open.png"
    "images/paste.png"
    "images/save.png"
)

qt_add_resources(${CMAKE_PROJECT_NAME} "mdi"
    PREFIX
        "/"
    FILES
        ${mdi_resource_files}
)

qt_finalize_target(${CMAKE_PROJECT_NAME})


#############################################################################################
# Defines
#############################################################################################
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    BA_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    BA_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    BA_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# using CMake on Visual Studio, debug using "BananaPlus with Qt6 %PATH%"
if(MSVC)
    get_target_property(c_qt_dir Qt6::Core IMPORTED_LOCATION_RELEASE)
    cmake_path(GET c_qt_dir PARENT_PATH c_qt_dir)
    message(STATUS  "configuring file launch with ${c_qt_dir}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/launch.vs.json.in ${CMAKE_SOURCE_DIR}/.vs/launch.vs.json @ONLY LF)
endif()
