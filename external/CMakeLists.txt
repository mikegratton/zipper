set(CMAKE_C_STANDARD 11)
find_package(ZLIB REQUIRED)
set(MINIZIP_SRC
    minizip/ioapi.c
    minizip/ioapi_buf.c
    minizip/ioapi_mem.c
    minizip/unzip.c
    minizip/zip.c
)

add_library(minizip ${MINIZIP_SRC})
target_include_directories(minizip PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/minizip)
target_link_libraries(minizip PRIVATE ZLIB::ZLIB)