set(NAME "stb_image")

add_library(${NAME} "stb_image.cpp")
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})