cmake_minimum_required(VERSION 3.19)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file"
    )
endif()

project(simpleDBMS-Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CLIENT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
find_package(asio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(CLIENT_VCPKG_TARGETS
    asio::asio
    nlohmann_json::nlohmann_json
)

file(GLOB_RECURSE CLIENT_SOURCE_FILES CONFIGURE_DEPENDS
    "${CLIENT_SOURCE_DIR}/*.cpp"
    "${CLIENT_SOURCE_DIR}/*.h"
)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(simpleDBMS-Client
    WIN32 MACOSX_BUNDLE
    ${CLIENT_SOURCE_FILES}
)

target_include_directories(simpleDBMS-Client
    PRIVATE
        ${CLIENT_SOURCE_DIR}
)

target_link_libraries(simpleDBMS-Client
    PRIVATE
        Qt::Core
        Qt::Widgets
        ${CLIENT_VCPKG_TARGETS}
)

include(GNUInstallDirs)

install(TARGETS simpleDBMS-Client
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET simpleDBMS-Client
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
