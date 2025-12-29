

# 递归查找所有源文件
file(GLOB_RECURSE SERVER_SRC
    ${CMAKE_SOURCE_DIR}/src/server/src/*.cpp
)

add_library(ServerModule OBJECT ${SERVER_SRC})
target_precompile_headers(ServerModule PRIVATE ${CMAKE_SOURCE_DIR}/src/public/include/pch.h)

target_include_directories(ServerModule
    PUBLIC
        ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_SOURCE_DIR}/src/server/include/
         ${CMAKE_SOURCE_DIR}/src/server/include/API/validators
        ${CMAKE_SOURCE_DIR}/src/server/include/API/Parser
        ${CMAKE_SOURCE_DIR}/external/tinyxml     
        ${CMAKE_SOURCE_DIR}/external/asio/include 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/cirSim/include
        ${CMAKE_SOURCE_DIR}/src/cirSim/include/CircuitElm/e-elements
        ${CMAKE_SOURCE_DIR}/src/cirSim/include/CircuitElm/elements
        ${CMAKE_SOURCE_DIR}/src/cirSim/include/CircuitElm/elements/components/meters


)