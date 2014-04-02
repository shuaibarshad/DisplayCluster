
# Copyright (c) 2013-2014, EPFL/Blue Brain Project
#                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                     Raphael Dumusc <raphael.dumusc@epfl.ch>
#
# Generates executables for the unit tests and defines a "tests" make target
# which runs all of them.

# TEST_LIBRARIES is used by CommonCTest.cmake to link tests against the given libraries
set(TEST_LIBRARIES
  ${DC_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  mock
)

# TODO Enable this later when https://github.com/Eyescale/CMake/pull/118 is merged?
#find_package(X11)
#if(X11_FOUND)
#  list(APPEND EXCLUDE_FROM_TEST_FILES
#    core/WebbrowserTests.cpp
#  )
#endif()

# recursively compile unit tests for *.cpp files in the current folder, linking with TEST_LIBRARIES
include(CommonCTest)
