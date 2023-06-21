add_library(restfslib SHARED restfs.hpp restutil.cpp restpool.cpp restfuse.cpp)
target_link_libraries(restfslib PRIVATE restclient-cpp)
target_link_libraries(restfslib PRIVATE cereal::cereal)
