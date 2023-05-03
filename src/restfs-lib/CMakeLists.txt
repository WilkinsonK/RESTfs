add_library(restfs-lib SHARED restfs.hpp restutil.cpp restpool.cpp restfuse.cpp)
target_link_libraries(restfs-lib PRIVATE restclient-cpp)
target_link_libraries(restfs-lib PRIVATE ${FUSE_LIBRARIES})
target_link_libraries(restfs-lib PRIVATE cereal::cereal)
