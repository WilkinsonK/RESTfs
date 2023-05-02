
# Include our header files for compile/link time.
set(SRC_SOURCES_HEADERS)
set(SRC_SOURCES_MAIN main.cpp)

# Add local libraries here.
add_subdirectory(restfs-lib restfs-lib)

# Include external libraries.
# set(TUCKER_LIBS_PRIVATE argparse::argparse restclient-cpp)
set(SRC_LIBS_LOCAL restfs-lib)

# Build and linkages for this project.
add_executable(${SRC_PROJECT_NAME} ${SRC_SOURCES_MAIN} ${SRC_SOURCES_HEADERS})

target_link_libraries(${SRC_PROJECT_NAME} PUBLIC restclient-cpp)
target_link_libraries(${SRC_PROJECT_NAME} PUBLIC ${SRC_LIBS_LOCAL})
