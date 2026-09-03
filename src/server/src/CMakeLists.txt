

# 递归查找所有源文件
file(GLOB_RECURSE SERVER_SRC
    ${CMAKE_SOURCE_DIR}/src/server/src/*.cpp
)

add_library(ServerModule OBJECT ${SERVER_SRC})

# The bundled websocketpp 0.8.2 supports C++11 to C++17 only: it declares
# constructors and destructors with a template-id, which C++20 no longer
# allows (GCC 11 and newer reject it with "template-id not allowed for
# destructor"). The server sources do not need C++20, so this module is
# built as C++17; CirSimModule keeps the project-wide C++20.
set_target_properties(ServerModule PROPERTIES CXX_STANDARD 17)
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