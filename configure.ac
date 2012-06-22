#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([libmtpipe], [0.01])

AC_CONFIG_HEADER(src/config.h)
#######################
# ARM
AC_ARG_ENABLE(
    [cross-prefix], 
    AS_HELP_STRING([--enable-cross-prefix=ARG],[cross compiler prefix i.e. arm_v5t_le-]),
    [CROSS_PREFIX=$enableval],
    [CROSS_PREFIX=])
AC_SUBST([CROSS_PREFIX])

#######################
# External Libraries
CC=["${CROSS_PREFIX}gcc"]
AC_SUBST([CC])

CXX=["${CROSS_PREFIX}g++"]
AC_SUBST([CXX])

STRIP=["${CROSS_PREFIX}strip"]
AC_SUBST([STRIP])

AR=["${CROSS_PREFIX}ar"]
AC_SUBST([AR])

RANLIB=["${CROSS_PREFIX}ranlib"]
AC_SUBST([RANLIB])

#####################
# Project directory
PROJECT_DIR=[$(pwd)]
AC_SUBST([PROJECT_DIR])

INCLUDE_DIR=$PROJECT_DIR/include
AC_SUBST([INCLUDE_DIR])

CINCLUDE_DIR=$PROJECT_DIR/external/cinclude/include
AC_SUBST([CINCLUDE_DIR])

LIB_DIR=$PROJECT_DIR/lib
AC_SUBST([LIB_DIR])

###############
# Target 
AC_ARG_ENABLE(
    [prefix], 
    AS_HELP_STRING([--prefix=ARG],[install prefix (default /usr/local/lib)]),
    [INSTALL_PREFIX=$enableval],
    [INSTALL_PREFIX=/usr/local/lib])
AC_SUBST([INSTALL_PREFIX])

##################
# pre-defined commands
INSTALL=install
AC_SUBST([INSTALL])
INSTALL_DOC=["${INSTALL} -o root -g root -p -m 644"]
AC_SUBST([INSTALL_DOC])
INSTALL_APP=["${INSTALL} -o root -g root -p -m 755"]

AC_SUBST([INSTALL_APP])

##################
# external dependency check
AC_PROG_CC($CC)

#AC_CHECK_FUNCS(semtimedop, 
#        [],
#        AC_MSG_ERROR([missing requred library function(s)]))

#AC_SEARCH_LIBS(clock_gettime, rt,
#        [],
#        AC_MSG_ERROR([missing posix time function(s)]))

# Checks for library functions.
AC_CONFIG_FILES([Makefile 
                 src/Makefile
                 test/Makefile])

AC_OUTPUT
