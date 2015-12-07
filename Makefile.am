# Put compiled object files into the same sub-directory as their sources
AUTOMAKE_OPTIONS = subdir-objects

# Point to our macro directory and pick up user flags from the environment
ACLOCAL_AMFLAGS  = -I macros ${ACLOCAL_FLAGS}

# Make warnings always fatal at distcheck time, and disable execution
# of tools that change files not installed by the package itself
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal --disable-update-mime-database

if ENABLE_DOCUMENTATION
doc_subdirs = docs
else
doc_subdirs =
endif

SUBDIRS = $(doc_subdirs)

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)
AM_CXXFLAGS = $(PROJECT_WXXFLAGS)

# TODO: Why do we still need to specify this for each executable that we build?
COMMON_CXXFLAGS = $(PROJECT_CFLAGS) $(PROJECT_WXXFLAGS)

include $(top_srcdir)/murraycdp/filelist.am

lib_LTLIBRARIES = libmurraycdp-@LIBMURRAYCDP_API_VERSION@.la
libmurraycdp_@LIBMURRAYCDP_API_VERSION@_la_SOURCES = $(cc_sources) $(h_sources_public)
libmurraycdp_@LIBMURRAYCDP_API_VERSION@_la_LDFLAGS = -no-undefined -version-info $(LIBMURRAYDP_SO_VERSION)
#libmurraycdp-_@LIBMURRAYCDP_API_VERSION@_la_LIBADD = $(LIBXMLXX_LIBS)

library_includedir = $(includedir)/$(LIBMURRAYCDP_MODULE_NAME)
nobase_library_include_HEADERS = $(h_sources_public)


check_PROGRAMS = \
  examples/murrayc_dp_bottom_up_fibonacci \
  examples/murrayc_dp_bottom_up_string_edit_distance \
  examples/murrayc_dp_bottom_up_string_substring_matching \
  examples/murrayc_dp_bottom_up_triple_step \
  examples/murrayc_dp_top_down_string_edit_distance \
  examples/murrayc_dp_top_down_make_change \
  examples/murrayc_dp_top_down_knapsack \
  examples/murrayc_dp_top_down_tsp \
  tests/test_vector_of_vectors \
  tests/test_tuple_cdr \
  tests/test_tuple_interlace

TESTS = $(check_PROGRAMS)

#List of source files needed to build the executable:

examples_murrayc_dp_bottom_up_string_edit_distance_SOURCES = \
	examples/dp_bottom_up_string_edit_distance/murrayc_dp_bottom_up_string_edit_distance.cc
examples_murrayc_dp_bottom_up_string_edit_distance_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_string_edit_distance_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_string_substring_matching_SOURCES = \
	examples/dp_bottom_up_string_substring_matching/murrayc_dp_bottom_up_string_substring_matching.cc
examples_murrayc_dp_bottom_up_string_substring_matching_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_string_substring_matching_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_triple_step_SOURCES = \
	examples/dp_bottom_up_triple_step/murrayc_dp_bottom_up_triple_step.cc
examples_murrayc_dp_bottom_up_triple_step_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_triple_step_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_fibonacci_SOURCES = \
	examples/dp_bottom_up_fibonacci/murrayc_dp_bottom_up_fibonacci.cc
examples_murrayc_dp_bottom_up_fibonacci_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_fibonacci_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_string_edit_distance_SOURCES = \
	examples/dp_top_down_string_edit_distance/murrayc_dp_top_down_string_edit_distance.cc
examples_murrayc_dp_top_down_string_edit_distance_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_string_edit_distance_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_make_change_SOURCES = \
	examples/dp_top_down_make_change/murrayc_dp_top_down_make_change.cc
examples_murrayc_dp_top_down_make_change_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_make_change_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_knapsack_SOURCES = \
	examples/dp_top_down_knapsack/murrayc_dp_top_down_knapsack.cc
examples_murrayc_dp_top_down_knapsack_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_knapsack_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_tsp_SOURCES = \
	examples/dp_top_down_tsp/murrayc_dp_top_down_tsp.cc \
	examples/dp_top_down_tsp/subset_iterator.h
examples_murrayc_dp_top_down_tsp_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_tsp_LDADD = \
	$(PROJECT_LIBS)

tests_test_vector_of_vectors_SOURCES = \
	tests/test_vector_of_vectors.cc
tests_test_vector_of_vectors_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
tests_test_vector_of_vectors_LDADD = \
	$(PROJECT_LIBS)

tests_test_tuple_cdr_SOURCES = \
	tests/test_tuple_cdr.cc
tests_test_tuple_cdr_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
tests_test_tuple_cdr_LDADD = \
	$(PROJECT_LIBS)

tests_test_tuple_interlace_SOURCES = \
	tests/test_tuple_interlace.cc
tests_test_tuple_interlace_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
tests_test_tuple_interlace_LDADD = \
	$(PROJECT_LIBS)


