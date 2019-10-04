AC_PREREQ(2.59)

AC_INIT([poppassd-ceti], [1.8.7], [poppassd-bugs@nym.hush.com])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([poppassd.c])
AC_CHECK_LIB(pam, pam_authenticate, [], [You need to install PAM development package: "apt-get install libpam0g"])

AC_LANG([C])
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])
AX_CFLAGS_WARN_ALL()

AC_CHECK_HEADERS([fcntl.h shadow.h stdlib.h string.h strings.h sys/time.h syslog.h])

AC_CHECK_HEADER([security/pam_appl.h], [], [AC_MSG_ERROR([You need to install PAM development package: "apt-get install libpam0g-dev" or "yum install pam-devel"])])
AC_CHECK_HEADER([security/pam_misc.h], [], [AC_MSG_ERROR([You need to install PAM development package: "apt-get install libpam0g-dev" or "yum install pam-devel"])])
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero strchr strdup])

AC_ARG_ENABLE(am-ldcflags, AS_HELP_STRING([--disable-am-ldcflags], [do not add various 'AM_CFLAGS/AM_LDFLAGS']))
if test "x$enable_am_ldcflags" != "xno"; then
   for flag in -fstack-protector-all -Wl,-z,relro -Wl,-z,now -fPIE -pie -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2; do
        my_save_cflags="$CFLAGS"
        CFLAGS=$flag
        AC_MSG_CHECKING([whether CC supports $flag])
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
            [AC_MSG_RESULT([yes])]
            [AM_CFLAGS="$AM_CFLAGS $flag"]
            [AC_MSG_RESULT([no])]
        )
        CFLAGS="$my_save_cflags"
    done
    AC_SUBST([AM_CFLAGS])
    AC_SUBST([AM_LDFLAGS])
fi



AC_OUTPUT
