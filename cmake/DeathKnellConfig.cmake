#.rst:
# FindDeathKnell
# -------
#
# Find libDeathKnell
# DeathKnell deals with file access and directory traversal.
##
# Result variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``DeathKnell_INCLUDE_DIRS``
#   where to find Death.h

#
# ``DeathKnell_LIBRARIES``
#   the libraries to link against to use libDeathKnell.
#   that includes libDeathKnell library files.
#
# ``DeathKnell_FOUND``
#   If false, do not try to use DeathKnell.
#
include(FindPackageHandleStandardArgs)
find_path(DeathKnell_INCLUDE_DIR Death.h)
find_library(DeathKnell_LIBRARY
            NAMES libDeathKnell DeathKnell)

find_package_handle_standard_args(DeathKnell  DEFAULT_MSG
            DeathKnell_INCLUDE_DIR DeathKnell_LIBRARY)
            
mark_as_advanced(DeathKnell_INCLUDE_DIR DeathKnell_LIBRARY)
set(DeathKnell_LIBRARIES ${DeathKnell_LIBRARY})
set(DeathKnell_INCLUDE_DIRS ${DeathKnell_INCLUDE_DIR})