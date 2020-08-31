cmake_minimum_required(VERSION 3.12)

project(poppassd VERSION 1.8.8)

include(CheckSymbolExists)
check_symbol_exists(strcasestr "string.h" HAVE_STRCASESTR)

configure_file(config.h.in config.h)
add_executable(poppassd poppassd.c config.h)
target_link_libraries(poppassd PRIVATE pam)

install(FILES poppassd TYPE SBIN)
install(FILES etc/pam/poppassd DESTINATION /etc/pam.d)
install(FILES etc/systemd/poppassd.service DESTINATION /etc/systemd/system)
install(FILES etc/systemd/poppassd.socket  DESTINATION /etc/systemd/system)
install(CODE "systemctl daemon-reload")
