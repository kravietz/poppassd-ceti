cmake_minimum_required(VERSION 3.12)

project(poppassd VERSION 1.8.8)

configure_file(config.h.in config.h)
add_executable(poppassd poppassd.c config.h)
target_link_libraries(poppassd PRIVATE pam)
