cmake_minimum_required(VERSION 3.10)
add_definitions(-D_FORTIFY_SOURCE=2)


# 设置项目名称和可执行文件名称
set(PROJECT_DISPLAY_NAME "嘉立创EDA仿真引擎")
project(${PROJECT_DISPLAY_NAME})


set(PROJECT_DISPLAY_NAME "嘉立创EDA仿真引擎")
project(${PROJECT_DISPLAY_NAME})
set(COMPANY_NAME "深圳嘉立创科技集团股份有限公司") 
set(FILE_DESCRIPTION "嘉立创EDA仿真引擎")
set(FILE_VERSION "1.12.1") 



# 设置编译缓存
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# 设置并行编译
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_JOB_POOL_COMPILE compile_job_pool)
    set(CMAKE_JOB_POOL_LINK link_job_pool)
    set(CMAKE_JOB_POOLS compile_job_pool=${N};link_job_pool=${N})
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_BUILD_TYPE release)

# 添加你的可执行文件或静态库名称
set(PROJECT_EXECUTABLE lceda-pro-sim-server)

# 设置编译器选项，可以根据需要调整
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

set(CMAKE_BUILD_RPATH "$ORIGIN")
set(CMAKE_INSTALL_RPATH "$ORIGIN")

# 添加头文件搜索路径
include_directories(
    ${CMAKE_SOURCE_DIR}/src/public/include
)

add_subdirectory(src/cirSim/src)
add_subdirectory(src/server/src)
# 设置图标路径
set(APP_ICON_PATH "${PROJECT_SOURCE_DIR}/image/favicon.ico")

if(NOT EXISTS "${APP_ICON_PATH}")
    message(WARNING "图标文件不存在: ${APP_ICON_PATH}")
endif()

string(REPLACE "." ";" VERSION_LIST ${FILE_VERSION})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)
set(FILE_VERSION_ABN "${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH}")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/app.rc [[
#pragma code_page(65001)
#include <winver.h>
IDI_ICON1 ICON "]] ${APP_ICON_PATH} [["
VS_VERSION_INFO VERSIONINFO
 FILEVERSION ]] ${FILE_VERSION_ABN} [[
 PRODUCTVERSION ]] ${FILE_VERSION_ABN} [[
 FILEFLAGSMASK 0x3fL
 FILEFLAGS 0x0L
 FILEOS VOS__WINDOWS32
 FILETYPE VFT_APP
 FILESUBTYPE 0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "080404b0"  // 中文（简体）
        BEGIN
            VALUE "CompanyName", "]] ${COMPANY_NAME} [["
            VALUE "FileDescription", "]] ${FILE_DESCRIPTION} [["
            VALUE "FileVersion", "]] ${FILE_VERSION} [["
            VALUE "ProductVersion", "]] ${FILE_VERSION} [["
            VALUE "ProductName", "]] ${PROJECT_DISPLAY_NAME} [["
            VALUE "InternalName", "]] ${PROJECT_EXECUTABLE} [["
            VALUE "OriginalFilename", "]] ${PROJECT_EXECUTABLE} [[.exe"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x804, 1200
    END
END
]])

# 生成可执行文件并启用预编译头
add_executable(${PROJECT_EXECUTABLE}
    ./src/server/server.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/app.rc  # 添加资源文件
)
target_precompile_headers(${PROJECT_EXECUTABLE} PRIVATE src/public/include/pch.h)

find_library(NGSPICE_LIBRARY NAMES ngspice PATHS ${CMAKE_SOURCE_DIR}/external/ngspice NO_DEFAULT_PATH)
find_library(TINYXML_LIB NAMES libtinyxml PATHS ${CMAKE_SOURCE_DIR}/external/tinyxml NO_DEFAULT_PATH)


# link_directories(${PROJECT_SOURCE_DIR}/bin)
# 链接外部库


if(WIN32)
    target_link_libraries(
        ${PROJECT_EXECUTABLE}
        ${TINYXML_LIB}
        ${NGSPICE_LIBRARY}
        CirSimModule
        ServerModule
        ws2_32
        mswsock
    )
    add_custom_command(TARGET ${PROJECT_EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/config"  
            "$<TARGET_FILE_DIR:${PROJECT_EXECUTABLE}>/"  
        COMMENT "Copy the config folder to the output directory..."
    )
else(Linux)
    target_link_libraries(
        ${PROJECT_EXECUTABLE}
        ${TINYXML_LIB}
        ${NGSPICE_LIBRARY}
        CirSimModule
        ServerModule
        pthread
        dl
        ncurses
        m 
        stdc++ 
        readline
        
    )
    add_custom_command(TARGET ${PROJECT_EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/config"  
            "$<TARGET_FILE_DIR:${PROJECT_EXECUTABLE}>/"  
        COMMENT "Copy the config folder to the output directory..."
    )
endif()