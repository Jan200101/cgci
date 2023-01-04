

list(APPEND
    SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/build.c
    ${CMAKE_CURRENT_SOURCE_DIR}/build.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cgci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cgci.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/context.c
    ${CMAKE_CURRENT_SOURCE_DIR}/context.h
    ${CMAKE_CURRENT_SOURCE_DIR}/env.c
    ${CMAKE_CURRENT_SOURCE_DIR}/env.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ui.h
)

set(CFLAGS
    -Wall -Wextra -Wfloat-equal
    -Wstrict-overflow=5 -Wunreachable-code
    -Wundef -Wcast-qual -Wconversion 
    -Wswitch-default -Wmissing-include-dirs
    -Wshadow -Wstrict-aliasing -Winit-self
    -Wcast-align -Wpointer-arith
    -Wno-unused-parameter -Wuninitialized
)



set(CMAKE_EXECUTABLE_SUFFIX ".cgi")
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC ${CFLAGS})
