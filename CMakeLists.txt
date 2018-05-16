cmake_minimum_required(VERSION 2.8)

project(sift)
include_directories("header")
link_libraries(m)
add_executable(${PROJECT_NAME} "main.c" "bmp.c" "mat.c" "dector.c" "type.c" "sift.c")
