# Put compiled object files into the same sub-directory as their sources
AUTOMAKE_OPTIONS = subdir-objects

# Point to our macro directory and pick up user flags from the environment
ACLOCAL_AMFLAGS  = -I macros ${ACLOCAL_FLAGS}

# Make warnings always fatal at distcheck time, and disable execution
# of tools that change files not installed by the package itself
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal --disable-update-mime-database

#SUBDIRS =

AM_CXXFLAGS = $(PROJECT_WXXFLAGS)

# TODO: Why do we still need to specify this for each executable that we build?
COMMON_CXXFLAGS = $(PROJECT_CFLAGS) $(PROJECT_WXXFLAGS)

bin_PROGRAMS = \
  murrayc_find_objects_in_image_with_disjoint_set \
  murrayc_greedy_make_change \
  murrayc_dp_make_change \
  murrayc_dependency_resolution \
  murrayc_quickselect \
  murrayc_subset_iteration \
  murrayc_dp_string_edit_distance \
  murrayc_dp_string_substring_matching

#List of source files needed to build the executable:
murrayc_find_objects_in_image_with_disjoint_set_SOURCES = \
	src/find_objects_in_image_with_disjoint_set/murrayc_find_objects_in_image_with_disjoint_set.cc
murrayc_find_objects_in_image_with_disjoint_set_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_find_objects_in_image_with_disjoint_set_LDADD = \
	$(PROJECT_LIBS)

murrayc_greedy_make_change_SOURCES = \
	src/greedy_make_change/murrayc_greedy_make_change.cc
murrayc_greedy_make_change_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_greedy_make_change_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_make_change_SOURCES = \
	src/dp_make_change/murrayc_dp_make_change.cc
murrayc_dp_make_change_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_make_change_LDADD = \
	$(PROJECT_LIBS)

murrayc_dependency_resolution_SOURCES = \
	src/dependency_resolution/murrayc_dependency_resolution.cc
murrayc_dependency_resolution_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dependency_resolution_LDADD = \
	$(PROJECT_LIBS)

murrayc_quickselect_SOURCES = \
	src/quickselect/murrayc_quickselect.cc
murrayc_quickselect_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_quickselect_LDADD = \
	$(PROJECT_LIBS)

murrayc_subset_iteration_SOURCES = \
	src/subset_iteration/murrayc_subset_iteration.cc
murrayc_subset_iteration_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_subset_iteration_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_string_edit_distance_SOURCES = \
	src/dp_string_edit_distance/murrayc_dp_string_edit_distance.cc
murrayc_dp_string_edit_distance_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_string_edit_distance_LDADD = \
	$(PROJECT_LIBS)

murrayc_dp_string_substring_matching_SOURCES = \
	src/dp_string_substring_matching/murrayc_dp_string_substring_matching.cc
murrayc_dp_string_substring_matching_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
murrayc_dp_string_substring_matching_LDADD = \
	$(PROJECT_LIBS)
