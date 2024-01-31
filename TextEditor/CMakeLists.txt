cmake_minimum_required(VERSION 3.24)

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
# Set the path to the ECM module directory
#set(ECM_MODULE_PATH "${CMAKE_SOURCE_DIR}/extra-cmake-modules")

# Add the ECM module directory to CMAKE_MODULE_PATH
#list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

# Find ECM package
# find_package(ECM REQUIRED NO_MODULE)

# Include ECM modules you need
#include(ECMInstallIcons)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network NetworkAuth LinguistTools)

#############################################################################################
# My binary
#############################################################################################
qt_add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    mainwindow.cpp mainwindow.h
    mdichild.cpp mdichild.h
    MANUAL_FINALIZATION
    authenticatorMicrosoft.h authenticatorMicrosoft.cpp
    helperstructures.h
    abstractauthenticator.h
    secrets.h
    authenticatorGoogle.h authenticatorGoogle.cpp
    abstractauthenticator.cpp
    ${translatable_sources}
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


#############################################################################################
# Define the target name
#set(TARGET_NAME "TextEditor")
#set(WINDEPLOYQT_EXECUTABLE "C:/Qt/6.6.1/msvc2019_64/bin/windeployqt.exe")

# Use find_program to locate windeployqt executable
#find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${WINDEPLOYQT_EXECUTABLE})

# Check if windeployqt executable was found
#if(NOT WINDEPLOYQT_EXECUTABLE)
#    message(FATAL_ERROR "windeployqt executable not found.")
#endif()

# Specify the target executable
#set(EXECUTABLE_PATH "${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/${TARGET_NAME}.exe")

# Add a custom command to run windeployqt during the build
#add_custom_command(
#    TARGET ${TARGET_NAME}
#    POST_BUILD
#    COMMAND ${WINDEPLOYQT_EXECUTABLE}
#            --release
#            --no-compiler-runtime
#            --no-translations
#            --dir ${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}
#            ${EXECUTABLE_PATH}
#    COMMENT "Running windeployqt"
#)

#message(STATUS "Executable path: ${EXECUTABLE_PATH}")
#message(STATUS "Windeployqt executable path: ${WINDEPLOYQT_EXECUTABLE}")

# Add a custom target that depends on the custom command
#add_custom_target(deploy_${TARGET_NAME} ALL DEPENDS ${EXECUTABLE_PATH})

#############################################################################################
# CPack Configuration
#############################################################################################


#set(CPACK_PACKAGE_NAME "TextEditor")
#set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
#set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
#set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
#set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
#set(CPACK_PACKAGE_INSTALL_DIRECTORY "TextEditor")
#set(c_currentDir ${CMAKE_CURRENT_SOURCE_DIR})
#set(c_executablePath ${EXECUTABLE_PATH})



# Specify components to include in the package
#install(TARGETS ${CMAKE_PROJECT_NAME}
#        DESTINATION ${CMAKE_INSTALL_BINDIR}
#        COMPONENT applications)

#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Texteditor.iss.in" "${CMAKE_CURRENT_BINARY_DIR}/Texteditor.iss" @ONLY)


# Set the path to Inno Setup Compiler 
#set(INNO_SETUP_COMPILER_PATH "C:/Program Files (x86)/Inno Setup 6/ISCC.exe" CACHE FILEPATH "Path to Inno Setup Compiler")

#if(WIN32)
    # Set QT_QPA_PLATFORM_PLUGIN_PATH for Windows
#    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#        COMMAND set QT_QPA_PLATFORM_PLUGIN_PATH=${CMAKE_BINARY_DIR}/bin_${CMAKE_BUILD_TYPE}/platforms
#    )
#    add_custom_target(set_qt_env_vars ALL DEPENDS ${CMAKE_PROJECT_NAME})
#endif()

#set(INNO_SETUP_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/Texteditor.iss")

# Add a custom target to build the Inno Setup installer
#add_custom_target(inno_setup_installer
#    COMMAND ${INNO_SETUP_COMPILER_PATH} ${INNO_SETUP_SCRIPT}
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#    COMMENT "Building Inno Setup installer"
#)

# Add dependency so that the installer is built when the main target is built
#add_dependencies(${CMAKE_PROJECT_NAME} inno_setup_installer)

# Define components
#set(CPACK_COMPONENTS_ALL applications)

# Set the CPack generator to NSIS (Nullsoft Scriptable Install System) for Windows
#set(CPACK_GENERATOR NSIS)

#include(CPack)

#############################################################################################
# Linguist Configuration
#############################################################################################
include(ExternalProject)

# Specify the source files for translation
set(translatable_sources
    mainwindow.cpp
)

# Add the custom target for lupdate
add_custom_target(translations)

add_custom_command(TARGET translations
    COMMAND ${CMAKE_COMMAND} -E echo "Running lupdate..."
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${CMAKE_PREFIX_PATH}/bin;$ENV{PATH}"
    COMMAND qt6_create_translation(${CMAKE_PROJECT_NAME}_qm ${translatable_sources} -ts ${CMAKE_CURRENT_BINARY_DIR}/TextEditor.ts)
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}  # Set the working directory
)

# Add the custom target as a dependency
# add_dependencies(${CMAKE_PROJECT_NAME} translations)

# Specify the target executable for which to generate .ts files
set(LUPDATE_TARGET ${CMAKE_PROJECT_NAME})

# Add the qt_add_lupdate command
#qt6_add_lupdate(${CMAKE_PROJECT_NAME}
#    TS_FILES ${CMAKE_CURRENT_BINARY_DIR}/TextEditor.ts 
#    SOURCES ${translatable_sources}  
#)

# Specify the target executable for which to generate .qm files
#set(LRELEASE_TARGET ${CMAKE_PROJECT_NAME})

# Add the qt6_add_lrelease command
#qt6_add_lrelease(${CMAKE_PROJECT_NAME}
#    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#    TS_FILES ${CMAKE_CURRENT_BINARY_DIR}/TextEditor.ts
#)

# Ensure lrelease depends on lupdate
# add_dependencies(${LRELEASE_TARGET} ${LUPDATE_TARGET})

# Install the generated .qm files
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/translations/${CMAKE_PROJECT_NAME}_qm.qm
#    DESTINATION ${CMAKE_INSTALL_PREFIX}/translations
#    COMPONENT translations
#)

# Add the custom target for lrelease
#add_custom_target(rtranslations DEPENDS translations)

# Add the custom target for lrelease
#add_custom_target(release_lrelease
#    COMMAND ${CMAKE_COMMAND} -E echo "Running lrelease..."
#    COMMAND ${CMAKE_COMMAND} -E env "PATH=${CMAKE_PREFIX_PATH}/bin;$ENV{PATH}"
#    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${CMAKE_PROJECT_NAME}_lrelease
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}  # Set the working directory
#)


# Make the lrelease target depend on the translations target
# add_dependencies(${CMAKE_PROJECT_NAME}_lrelease translations)

# Add the qt_add_lrelease command
#OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/translations" 
#qt6_add_lrelease(${CMAKE_PROJECT_NAME}
#    TS_FILES ${CMAKE_CURRENT_BINARY_DIR}/TextEditor.ts
#    SOURCES ${translatable_sources}
#    OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations"
#    QM_FILES_OUTPUT_VARIABLE qm_files
#)

set(CMAKE_LIBRARY_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/libs/sources)
 
set(c_kde_cmakeArgs
    -GNinja
    -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)
 
#############################################################################################
# KDE Extra CMake Modules (ECM)
# https://api.kde.org/
# Adds to the modules provided by CMake, including ones used by find_package()
# to find common software, and common build settings used in software produced by the
# KDE community.
# https://github.com/KDE/extra-cmake-modules/tags
#############################################################################################


set(zlib_version "1.3.1")

file(ARCHIVE_EXTRACT
    INPUT ${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib-1.3.1.tar.gz
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
# Download and extract zlib



# Set the URL for zlib download
set(zlib_url "https://zlib.net/zlib-${zlib_version}.tar.gz")

# Add ExternalProject for zlib
ExternalProject_Add(
    zlib_external
    URL ${zlib_url}
    URL_HASH SHA256=9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/downloads
    BUILD_IN_SOURCE ON
    PREFIX ${CMAKE_BINARY_DIR}/zlib
    INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Zlib installation not required for header-only library."
    DOWNLOAD_EXTRACT_TIMESTAMP False
)


set(c_kde_cmakeArgs
    -GNinja
    -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

set(c_extraModules_name extra-cmake-modules-5.248.0)

# Extract the source archive
file(ARCHIVE_EXTRACT
    INPUT ${CMAKE_CURRENT_SOURCE_DIR}/libs/${c_extraModules_name}.tar.gz
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/libs
)

# Ensure the source directory is set correctly after extraction
set(c_extraModules_source_dir ${CMAKE_CURRENT_BINARY_DIR}/libs/${c_extraModules_name})

# ExternalProject_Add command
ExternalProject_Add(
    c_extra_modules
    SOURCE_DIR ${c_extraModules_source_dir}
    DEPENDS  zlib_external
    BUILD_IN_SOURCE ON
    LIST_SEPARATOR |
    CMAKE_ARGS
        ${c_kde_cmakeArgs}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_SOURCES}/extra-cmake-modules
)

#add c_extra_modules to the prefix path
set(t_kde_prefixPath ${CMAKE_PREFIX_PATH})
list(APPEND t_kde_prefixPath
	${CMAKE_LIBRARY_SOURCES}/extra-cmake-modules
)
string(REPLACE ";" "|" t_kde_prefixPath "${t_kde_prefixPath}")
 
 
#set the common cmake args for KDE projects
list(APPEND c_kde_cmakeArgs
    -DQT_MAJOR_VERSION=${QT_DEFAULT_MAJOR_VERSION}
    -DCMAKE_PREFIX_PATH=${t_kde_prefixPath}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DZLIB_INCLUDE_DIR=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include
)
if(APPLE)
    string(REPLACE ";" "|" t_arch "${CMAKE_OSX_ARCHITECTURES}")
    list(APPEND c_kde_cmakeArgs
        -DCMAKE_OSX_ARCHITECTURES=${t_arch}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    )
endif()
 
set(c_kuf_args ${c_kde_cmakeArgs})
list(APPEND c_kuf_args
    -DENABLE_DOCS=OFF
    -DENABLE_CONSOLE=OFF
    -DENABLE_CLI=OFF
    -DENABLE_PHP=OFF
    -DENABLE_PHP_UNIT=OFF
    -DENABLE_SURVEY_TARGET_EXPRESSIONS=OFF
)

ExternalProject_Add(kuserfeedback_make
    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/kuserfeedback
    URL https://invent.kde.org/frameworks/kuserfeedback/-/archive/v5.248.0/kuserfeedback-v5.248.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    DEPENDS c_extra_modules
    BUILD_IN_SOURCE ON
    LIST_SEPARATOR |
    CMAKE_ARGS ${c_kuf_args}
)


# Link your main project with the external project
add_dependencies(${CMAKE_PROJECT_NAME} kuserfeedback_make)

# Include directories and libraries from the external project
#target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/kuserfeedback-install/include)
#target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/kuserfeedback-install/lib/libKUserFeedback.a)

# Look at the possiblity to use FetchContent()n funtcion which makes the content available immediately, thus allowing the configure step to use the content in commands 