add_library(restfs-lib SHARED restfs.hpp restutil.cpp restpool.cpp restfuse.cpp)
target_link_libraries(restfs-lib PUBLIC restclient-cpp)
target_link_libraries(restfs-lib PUBLIC ${FUSE_LIBRARIES})
