AC_INIT([poppassd-ceti], [1.8.6], [poppassd-bugs@nym.hush.com], [https://github.com/kravietz/poppassd-ceti])
AC_CONFIG_SRCDIR([poppassd.c])
AC_CHECK_LIB(pam, pam_authenticate, [], [You need to PAM development package: "apt-get install libpam0g"])

AC_LANG([C])
AC_PROG_CC
AC_PROG_INSTALL

AC_CHECK_HEADERS([fcntl.h shadow.h stdlib.h string.h strings.h sys/time.h syslog.h])

AC_CHECK_HEADER([security/pam_appl.h], [], [AC_MSG_ERROR([You need to PAM development package: "apt-get install libpam0g-dev"])])
AC_CHECK_HEADER([security/pam_misc.h], [], [AC_MSG_ERROR([You need to PAM development package: "apt-get install libpam0g-dev"])])

AX_CHECK_COMPILE_FLAG(["-fstack-protector-all"], [AX_APPEND_FLAG(["-fstack-protector-all"],[CFLAGS])], [])
AX_CFLAGS_WARN_ALL()

AX_APPEND_FLAG(["-O3"],[CFLAGS])
AX_APPEND_FLAG(["-D_FORTIFY_SOURCE=2"],[CFLAGS])

AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero strchr strdup])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
