
# Include our header files for compile/link time.
set(SRC_SOURCES_HEADERS)
set(SRC_SOURCES_MAIN main.cpp)

# Add local libraries here.
add_subdirectory(argoptlib argoptlib)
add_subdirectory(restfslib restfslib)

# Include external libraries.
set(SRC_LIBS_LOCAL restfslib argoptlib)

# Build and linkages for this project.
add_executable(${SRC_PROJECT_NAME} ${SRC_SOURCES_MAIN} ${SRC_SOURCES_HEADERS})

target_link_libraries(${SRC_PROJECT_NAME} PRIVATE cereal::cereal)
target_link_libraries(${SRC_PROJECT_NAME} PRIVATE ${FUSE_LIBRARIES})
target_link_libraries(${SRC_PROJECT_NAME} PUBLIC ${SRC_LIBS_LOCAL})
