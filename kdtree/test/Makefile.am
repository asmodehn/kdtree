check_PROGRAMS = kdtree-check kdtree-perf
AM_CPPFLAGS = -Wall -ansi -pedantic -I../src $(all_includes)
kdtree_check_SOURCES = main.cc
kdtree_check_CPPFLAGS = $(AM_CPPFLAGS) -DCHECK
kdtree_perf_SOURCES = main.cc
TESTS = kdtree-check kdtree-perf
