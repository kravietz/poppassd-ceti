cmake_minimum_required(VERSION 3.12)

project(poppassd VERSION 1.8.10)

include(CheckSymbolExists)
check_symbol_exists(strcasestr "string.h" HAVE_STRCASESTR)

# Linux specific include, skip on FreeBSD
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE(security/pam_misc.h HAVE_PAM_MISC)

configure_file(config.h.in config.h)
add_executable(poppassd poppassd.c config.h)
target_link_libraries(poppassd PRIVATE pam)

# binary installed in /usr/local/sbin/poppassd on all systems
install(FILES poppassd TYPE SBIN PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

# we only install the service on systemd OS
# on legacy Linux and FreeBSD you need to use inetd/xinetd

find_file(SYSTEMD system.conf PATHS /etc/systemd NO_DEFAULT_PATH)
if (SYSTEMD)
	install(FILES etc/pam/poppassd DESTINATION /etc/pam.d)
	install(FILES etc/systemd/poppassd.service DESTINATION /etc/systemd/system)
	install(FILES etc/systemd/poppassd.socket  DESTINATION /etc/systemd/system)
	install(CODE "execute_process(COMMAND systemctl daemon-reload)")
	install(CODE "execute_process(COMMAND systemctl enable poppassd.socket)")
	install(CODE "execute_process(COMMAND systemctl start poppassd.socket)")
else()
	message(STATUS "Skiping service installation - this is a non-systemd OS.")
endif()

enable_testing()

add_test(NAME Init_test_user 	COMMAND bash test/init-user.sh)
add_test(NAME Password_change	COMMAND expect test/successful-password-change.expect)
add_test(NAME Old_pass_wrong 	COMMAND expect test/old-pass-incorrect.expect)
add_test(NAME Invalid_user   	COMMAND expect test/user-does-not-exist.expect)
