cmake_minimum_required (VERSION 3.1)
project(habitc)

add_subdirectory(src)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
