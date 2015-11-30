# Put compiled object files into the same sub-directory as their sources
AUTOMAKE_OPTIONS = subdir-objects

# Point to our macro directory and pick up user flags from the environment
ACLOCAL_AMFLAGS  = -I macros ${ACLOCAL_FLAGS}

# Make warnings always fatal at distcheck time, and disable execution
# of tools that change files not installed by the package itself
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal --disable-update-mime-database

#SUBDIRS =

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)
AM_CXXFLAGS = $(PROJECT_WXXFLAGS)

# TODO: Why do we still need to specify this for each executable that we build?
COMMON_CXXFLAGS = $(PROJECT_CFLAGS) $(PROJECT_WXXFLAGS)

bin_PROGRAMS = \
  murrayc_dp_make_change \
  murrayc_dp_string_edit_distance \
  murrayc_dp_string_substring_matching

#List of source files needed to build the executable:

murrayc_dp_make_change_SOURCES = \
	examples/dp_make_change/murrayc_dp_make_change.cc
murrayc_dp_make_change_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_make_change_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_string_edit_distance_SOURCES = \
	examples/dp_string_edit_distance/murrayc_dp_string_edit_distance.cc
murrayc_dp_string_edit_distance_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_string_edit_distance_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_string_substring_matching_SOURCES = \
	examples/dp_string_substring_matching/murrayc_dp_string_substring_matching.cc
murrayc_dp_string_substring_matching_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_string_substring_matching_LDADD = \
	$(PROJECT_LIBS)
