AC_INIT([murrayc-dp-algorithms],[0.0.1])
AC_PREREQ(2.63)

AC_CONFIG_SRCDIR([examples/dp_top_down_make_change/murrayc_dp_top_down_make_change.cc])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([macros])
$AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.10 -Wno-portability no-define nostdinc tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])

LT_PREREQ([2.2.6])
LT_INIT([win32-dll])

MM_PREREQ([0.9.8])
MM_INIT_MODULE([libmurraycdp-1.0])

# Tell mm-common-prepare to copy the mm-common .pl scripts
# and some other files into docs/, and use them from there,
# so we can dist them to avoid a tarball-build dependency.
MM_CONFIG_DOCTOOL_DIR([docs])

MM_ARG_ENABLE_DOCUMENTATION
#For instance: MM_ARG_WITH_TAGFILE_DOC([libstdc++.tag], [mm-common-libstdc++])

# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
AC_SUBST([LIBMURRAYCDP_SO_VERSION], [0:0:0])

#PKG_CHECK_MODULES([PROJECT], [])

# See http://www.gnu.org/software/autoconf-archive/ax_boost_base.html
# Boost System is needed by Boost Timer
AX_BOOST_BASE
AX_BOOST_SYSTEM
AX_BOOST_TIMER

AC_LANG([C++])
MM_ARG_ENABLE_WARNINGS([PROJECT_WXXFLAGS],
                       [-Wall],
                       [-pedantic -Wall -Wextra -Wsuggest-override -Wshadow -Wformat-security -Wno-missing-field-initializers -fsanitize=undefined -ftrapv -DGSEAL_ENABLE],
                       [])


AC_CONFIG_FILES([Makefile
  docs/Makefile
  docs/reference/Doxyfile])

AC_OUTPUT
