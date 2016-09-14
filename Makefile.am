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
COMMON_CXXFLAGS = $(PROJECT_CFLAGS) $(PROJECT_WXXFLAGS) $(BOOST_CPPFLAGS)

include $(top_srcdir)/murraycdp/filelist.am

lib_LTLIBRARIES = libmurraycdp-@LIBMURRAYCDP_API_VERSION@.la
libmurraycdp_@LIBMURRAYCDP_API_VERSION@_la_SOURCES = $(cc_sources) $(h_sources_public)
libmurraycdp_@LIBMURRAYCDP_API_VERSION@_la_LDFLAGS = -no-undefined -version-info $(LIBMURRAYCDP_SO_VERSION)
#libmurraycdp-_@LIBMURRAYCDP_API_VERSION@_la_LIBADD = $(LIBXMLXX_LIBS)

library_includedir = $(includedir)/$(LIBMURRAYCDP_MODULE_NAME)
nobase_library_include_HEADERS = $(h_sources_public)


check_PROGRAMS = \
  examples/murrayc_dp_bottom_up_fibonacci \
  examples/murrayc_dp_bottom_up_sequence_alignment \
  examples/murrayc_dp_bottom_up_knapsack \
  examples/murrayc_dp_bottom_up_lcs \
  examples/murrayc_dp_bottom_up_optimal_binary_search_tree \
  examples/murrayc_dp_bottom_up_rod_cutting \
  examples/murrayc_dp_bottom_up_string_edit_distance \
  examples/murrayc_dp_bottom_up_string_substring_matching \
  examples/murrayc_dp_bottom_up_triple_step \
  examples/murrayc_dp_top_down_fibonacci \
  examples/murrayc_dp_top_down_knapsack \
  examples/murrayc_dp_top_down_lcs \
  examples/murrayc_dp_top_down_string_edit_distance \
  examples/murrayc_dp_top_down_make_change \
  examples/murrayc_dp_top_down_parenthesization \
  examples/murrayc_dp_top_down_parse_context_free_grammar \
  examples/murrayc_dp_top_down_rod_cutting \
  examples/murrayc_dp_top_down_tsp \
  tests/test_vector_of_vectors

TESTS = $(check_PROGRAMS)

#List of source files needed to build the executable:

examples_murrayc_dp_bottom_up_fibonacci_SOURCES = \
	examples/dp_bottom_up_fibonacci/murrayc_dp_bottom_up_fibonacci.cc
examples_murrayc_dp_bottom_up_fibonacci_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_fibonacci_LDADD = \
	$(PROJECT_LIBS) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_TIMER_LIB)

examples_murrayc_dp_bottom_up_knapsack_SOURCES = \
	examples/dp_bottom_up_knapsack/murrayc_dp_bottom_up_knapsack.cc
examples_murrayc_dp_bottom_up_knapsack_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_knapsack_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_lcs_SOURCES = \
	examples/dp_bottom_up_lcs/murrayc_dp_bottom_up_lcs.cc
examples_murrayc_dp_bottom_up_lcs_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_lcs_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_optimal_binary_search_tree_SOURCES = \
	examples/dp_bottom_up_optimal_binary_search_tree/murrayc_dp_bottom_up_optimal_binary_search_tree.cc
examples_murrayc_dp_bottom_up_optimal_binary_search_tree_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_optimal_binary_search_tree_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_sequence_alignment_SOURCES = \
	examples/dp_bottom_up_sequence_alignment/murrayc_dp_bottom_up_sequence_alignment.cc
examples_murrayc_dp_bottom_up_sequence_alignment_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_sequence_alignment_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_string_edit_distance_SOURCES = \
	examples/dp_bottom_up_string_edit_distance/murrayc_dp_bottom_up_string_edit_distance.cc
examples_murrayc_dp_bottom_up_string_edit_distance_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_string_edit_distance_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_bottom_up_rod_cutting_SOURCES = \
	examples/dp_bottom_up_rod_cutting/murrayc_dp_bottom_up_rod_cutting.cc
examples_murrayc_dp_bottom_up_rod_cutting_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_bottom_up_rod_cutting_LDADD = \
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

examples_murrayc_dp_top_down_fibonacci_SOURCES = \
	examples/dp_top_down_fibonacci/murrayc_dp_top_down_fibonacci.cc
examples_murrayc_dp_top_down_fibonacci_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_fibonacci_LDADD = \
	$(PROJECT_LIBS) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_TIMER_LIB)

examples_murrayc_dp_top_down_knapsack_SOURCES = \
	examples/dp_top_down_knapsack/murrayc_dp_top_down_knapsack.cc
examples_murrayc_dp_top_down_knapsack_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_knapsack_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_lcs_SOURCES = \
	examples/dp_top_down_lcs/murrayc_dp_top_down_lcs.cc
examples_murrayc_dp_top_down_lcs_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_lcs_LDADD = \
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

examples_murrayc_dp_top_down_parenthesization_SOURCES = \
	examples/dp_top_down_parenthesization/murrayc_dp_top_down_parenthesization.cc
examples_murrayc_dp_top_down_parenthesization_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_parenthesization_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_parse_context_free_grammar_SOURCES = \
	examples/dp_top_down_parse_context_free_grammar/murrayc_dp_top_down_parse_context_free_grammar.cc
examples_murrayc_dp_top_down_parse_context_free_grammar_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_parse_context_free_grammar_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_rod_cutting_SOURCES = \
	examples/dp_top_down_rod_cutting/murrayc_dp_top_down_rod_cutting.cc
examples_murrayc_dp_top_down_rod_cutting_CXXFLAGS = \
	$(COMMON_CXXFLAGS)
examples_murrayc_dp_top_down_rod_cutting_LDADD = \
	$(PROJECT_LIBS)

examples_murrayc_dp_top_down_tsp_SOURCES = \
	examples/dp_top_down_tsp/murrayc_dp_top_down_tsp.cc
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


