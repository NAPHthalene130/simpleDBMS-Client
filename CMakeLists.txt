cmake_minimum_required(VERSION 3.16) # 建议至少 3.16 以支持更好的 Qt6 特性

# 1. 确保 vcpkg 工具链生效
set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# 2. 设置 Qt6 路径
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.3/msvc2022_64")

project(simpleDBMS-Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. 扫描源文件
set(CLIENT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE CLIENT_SOURCE_FILES CONFIGURE_DEPENDS
        "${CLIENT_SOURCE_DIR}/*.cpp"
        "${CLIENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h"
)

# 4. 查找 Qt 组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

# 5. 创建可执行程序 (去掉多余的 my_executable)
qt_add_executable(simpleDBMS-Client
        WIN32
        ${CLIENT_SOURCE_FILES}
)

# 6. 包含路径与库链接 (修正了括号问题)
target_include_directories(simpleDBMS-Client PRIVATE ${CLIENT_SOURCE_DIR})

target_link_libraries(simpleDBMS-Client
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
)

# 7. 自动执行方案三 (windeployqt)
# 这样你每次构建完，DLL 都会自动出现在 cmake-build-debug 文件夹里
get_target_property(QT_DIR Qt6::Core LOCATION) # 自动获取 Qt 路径
get_filename_component(QT_BIN_DIR "${QT_DIR}" DIRECTORY)
set(WINDEPLOYQT_EXECUTABLE "${QT_BIN_DIR}/windeployqt.exe")

add_custom_command(TARGET simpleDBMS-Client POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE} --no-translations "$<TARGET_FILE:simpleDBMS-Client>"
        COMMENT "正在自动部署 Qt 依赖库 (windeployqt)..."
)
