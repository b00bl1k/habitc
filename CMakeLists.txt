cmake_minimum_required (VERSION 3.1)
project(habitc)

add_subdirectory(src)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
