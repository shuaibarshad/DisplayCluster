
# Copyright (c) 2013-2014, EPFL/Blue Brain Project
#                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                     Raphael Dumusc <raphael.dumusc@epfl.ch>
#
# Copy and configure resoureces used by the tests

# Copy the files needed by the tests to the build directory
set(TEST_RESOURCES webgl_interaction.html select_test.htm
  configuration.xml configuration_default.xml)

foreach(FILE ${TEST_RESOURCES})
  file(COPY ${FILE} DESTINATION ${CMAKE_BINARY_DIR}/tests/cpp)
endforeach()
