cmake_minimum_required(VERSION 3.10)
project(PLang VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(ptranspiler src/main.c)

install(TARGETS ptranspiler RUNTIME DESTINATION bin)

option(BUILD_EXAMPLES "Build example runner target" ON)
if(BUILD_EXAMPLES)
  add_custom_target(examples ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Examples available in ${CMAKE_SOURCE_DIR}/examples"
    COMMENT "Examples target (no compile step)"
  )
endif()
