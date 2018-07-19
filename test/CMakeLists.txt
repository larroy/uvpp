CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(test-uvpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic -pthread -std=c++11 -Wl,--no-as-needed")
SET(CMAKE_BUILD_TYPE Debug)

INCLUDE_DIRECTORIES(
    ../include
    ${PROJECT_SOURCE_DIR}
)

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp Server.cpp TcpConnection.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} uv)
