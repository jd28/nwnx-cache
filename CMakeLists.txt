find_package(SparseHash REQUIRED)

message(WARNING ${SPARSEHASH_INCLUDE_DIR})

include_directories(
  .
  ${SPARSEHASH_INCLUDE_DIR}
)

add_module(cache
    plugin-cache
    NWNXCache
    pluginlink.h
    ObjectCache.h
)

