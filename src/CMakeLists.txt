file(GLOB_RECURSE source CONFIGURE_DEPENDS "*.c" "*.cpp")
add_library(zipper ${source})
target_include_directories(zipper PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(zipper PRIVATE minizip)
