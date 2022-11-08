#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([File Sender], [0.0.1], [pashev.igor@gmail.com])
AM_INIT_AUTOMAKE([dist-xz foreign])
AC_CONFIG_SRCDIR([server.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h netinet/in.h stddef.h stdlib.h string.h sys/socket.h unistd.h sys/sendfile.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct sf_hdtr.hdr_cnt], [], [],
[
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
])

# Checks for library functions.
AC_FUNC_MALLOC
AC_SEARCH_LIBS([socket], [socket])
AC_SEARCH_LIBS([sendfile], [sendfile])
AC_CHECK_FUNCS([memset sendfile strerror])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT()

