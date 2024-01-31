
cmake_minimum_required(VERSION 3.21)

##################################################################
# Create the target for installer generation
# Show files in the IDE
##################################################################
set(m_distrName Create_installers)


add_custom_target(${m_distrName}
    COMMAND
        ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package
    SOURCES
        windows/installerBanana.iss.in
        windows/windeploy.cmake.in
)


##################################################################
# variables for CPACK and for .in files
##################################################################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ma Summary")
set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Banana.ch SA")
set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/src/resources/app-iconset/${c_icon_name})
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_HOMEPAGE_URL https://www.banana.ch)
set(CPACK_PACKAGE_FILE_NAME ${c_exe_name}-${CMAKE_PROJECT_VERSION}-setup)
set(CPACK_PACKAGE_DISPLAY_NAME ${c_exe_displayName})

#include the BananaPlus exe in the cpack generator
install(
    TARGETS ${CMAKE_PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${c_exe_name} 
)


# Use cmake to find vcredist dlls
set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)
set(CMAKE_INSTALL_DEBUG_LIBRARIES FALSE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)


# Innosetup configuration
set(innosetup_iss_in_file ${CMAKE_CURRENT_SOURCE_DIR}/windows/installerBanana.iss.in)


#find windeployqt
find_program(WINDEPLOYQT_EXE windeployqt)

#find innosetup compiler
set(innosetupPaths 
    "$ENV{PROGRAMFILES}/Inno Setup 6"
    "$ENV{ProgramFiles\(x86\)}/Inno Setup 6"
)
find_program(INNOSETUP_COMPILER ISCC.exe 
    PATHS ${innosetupPaths}
)


##################################################################
# call the external generator for windows
##################################################################
set(CPACK_GENERATOR "External")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/windows/windeploy.cmake.in 
    ${CMAKE_BINARY_DIR}/windeploy.cmake @ONLY
)
set(CPACK_EXTERNAL_PACKAGE_SCRIPT ${CMAKE_BINARY_DIR}/windeploy.cmake)
set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)
set(CPACK_MONOLITHIC_INSTALL TRUE)