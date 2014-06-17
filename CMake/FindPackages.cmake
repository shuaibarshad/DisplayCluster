# generated by Buildyard, do not edit.

include(System)
list(APPEND FIND_PACKAGES_DEFINES ${SYSTEM})
find_package(PkgConfig)

set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
if(PKG_CONFIG_EXECUTABLE)
  find_package(Boost 1.41.0 COMPONENTS program_options date_time serialization unit_test_framework regex)
  if((NOT Boost_FOUND) AND (NOT BOOST_FOUND))
    pkg_check_modules(Boost Boost>=1.41.0)
  endif()
  if((NOT Boost_FOUND) AND (NOT BOOST_FOUND))
    message(FATAL_ERROR "Could not find Boost COMPONENTS program_options date_time serialization unit_test_framework regex")
  endif()
else()
  find_package(Boost 1.41.0  REQUIRED program_options date_time serialization unit_test_framework regex)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(LibJpegTurbo 1.2.1)
  if((NOT LibJpegTurbo_FOUND) AND (NOT LIBJPEGTURBO_FOUND))
    pkg_check_modules(LibJpegTurbo LibJpegTurbo>=1.2.1)
  endif()
  if((NOT LibJpegTurbo_FOUND) AND (NOT LIBJPEGTURBO_FOUND))
    message(FATAL_ERROR "Could not find LibJpegTurbo")
  endif()
else()
  find_package(LibJpegTurbo 1.2.1  REQUIRED)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(Qt4 4.6 COMPONENTS QtCore QtGui QtNetwork QtOpenGL QtXml QtXmlPatterns QtSvg QtWebKit)
  if((NOT Qt4_FOUND) AND (NOT QT4_FOUND))
    pkg_check_modules(Qt4 Qt4>=4.6)
  endif()
  if((NOT Qt4_FOUND) AND (NOT QT4_FOUND))
    message(FATAL_ERROR "Could not find Qt4 COMPONENTS QtCore QtGui QtNetwork QtOpenGL QtXml QtXmlPatterns QtSvg QtWebKit")
  endif()
else()
  find_package(Qt4 4.6  REQUIRED QtCore QtGui QtNetwork QtOpenGL QtXml QtXmlPatterns QtSvg QtWebKit)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(MPI )
  if((NOT MPI_FOUND) AND (NOT MPI_FOUND))
    pkg_check_modules(MPI MPI)
  endif()
else()
  find_package(MPI  )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(Poppler 0.24 COMPONENTS Qt4)
  if((NOT Poppler_FOUND) AND (NOT POPPLER_FOUND))
    pkg_check_modules(Poppler Poppler>=0.24)
  endif()
else()
  find_package(Poppler 0.24  COMPONENTS Qt4)
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(GLUT )
  if((NOT GLUT_FOUND) AND (NOT GLUT_FOUND))
    pkg_check_modules(GLUT GLUT)
  endif()
else()
  find_package(GLUT  )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(OpenGL )
  if((NOT OpenGL_FOUND) AND (NOT OPENGL_FOUND))
    pkg_check_modules(OpenGL OpenGL)
  endif()
else()
  find_package(OpenGL  )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(TUIO 1.4)
  if((NOT TUIO_FOUND) AND (NOT TUIO_FOUND))
    pkg_check_modules(TUIO TUIO>=1.4)
  endif()
else()
  find_package(TUIO 1.4 )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(FFMPEG )
  if((NOT FFMPEG_FOUND) AND (NOT FFMPEG_FOUND))
    pkg_check_modules(FFMPEG FFMPEG)
  endif()
else()
  find_package(FFMPEG  )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(OpenMP )
  if((NOT OpenMP_FOUND) AND (NOT OPENMP_FOUND))
    pkg_check_modules(OpenMP OpenMP)
  endif()
else()
  find_package(OpenMP  )
endif()

if(PKG_CONFIG_EXECUTABLE)
  find_package(FCGI )
  if((NOT FCGI_FOUND) AND (NOT FCGI_FOUND))
    pkg_check_modules(FCGI FCGI)
  endif()
else()
  find_package(FCGI  )
endif()


if(EXISTS ${CMAKE_SOURCE_DIR}/CMake/FindPackagesPost.cmake)
  include(${CMAKE_SOURCE_DIR}/CMake/FindPackagesPost.cmake)
endif()

if(BOOST_FOUND)
  set(Boost_name BOOST)
  set(Boost_FOUND TRUE)
elseif(Boost_FOUND)
  set(Boost_name Boost)
  set(BOOST_FOUND TRUE)
endif()
if(Boost_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_BOOST)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Boost")
  link_directories(${${Boost_name}_LIBRARY_DIRS})
  if(NOT "${${Boost_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(SYSTEM ${${Boost_name}_INCLUDE_DIRS})
  endif()
endif()

if(LIBJPEGTURBO_FOUND)
  set(LibJpegTurbo_name LIBJPEGTURBO)
  set(LibJpegTurbo_FOUND TRUE)
elseif(LibJpegTurbo_FOUND)
  set(LibJpegTurbo_name LibJpegTurbo)
  set(LIBJPEGTURBO_FOUND TRUE)
endif()
if(LibJpegTurbo_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_LIBJPEGTURBO)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} LibJpegTurbo")
  link_directories(${${LibJpegTurbo_name}_LIBRARY_DIRS})
  if(NOT "${${LibJpegTurbo_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${LibJpegTurbo_name}_INCLUDE_DIRS})
  endif()
endif()

if(QT4_FOUND)
  set(Qt4_name QT4)
  set(Qt4_FOUND TRUE)
elseif(Qt4_FOUND)
  set(Qt4_name Qt4)
  set(QT4_FOUND TRUE)
endif()
if(Qt4_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_QT4)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Qt4")
  link_directories(${${Qt4_name}_LIBRARY_DIRS})
  if(NOT "${${Qt4_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(SYSTEM ${${Qt4_name}_INCLUDE_DIRS})
  endif()
endif()

if(MPI_FOUND)
  set(MPI_name MPI)
  set(MPI_FOUND TRUE)
elseif(MPI_FOUND)
  set(MPI_name MPI)
  set(MPI_FOUND TRUE)
endif()
if(MPI_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_MPI)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} MPI")
  link_directories(${${MPI_name}_LIBRARY_DIRS})
  if(NOT "${${MPI_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${MPI_name}_INCLUDE_DIRS})
  endif()
endif()

if(POPPLER_FOUND)
  set(Poppler_name POPPLER)
  set(Poppler_FOUND TRUE)
elseif(Poppler_FOUND)
  set(Poppler_name Poppler)
  set(POPPLER_FOUND TRUE)
endif()
if(Poppler_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_POPPLER)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Poppler")
  link_directories(${${Poppler_name}_LIBRARY_DIRS})
  if(NOT "${${Poppler_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${Poppler_name}_INCLUDE_DIRS})
  endif()
endif()

if(GLUT_FOUND)
  set(GLUT_name GLUT)
  set(GLUT_FOUND TRUE)
elseif(GLUT_FOUND)
  set(GLUT_name GLUT)
  set(GLUT_FOUND TRUE)
endif()
if(GLUT_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_GLUT)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} GLUT")
  link_directories(${${GLUT_name}_LIBRARY_DIRS})
  if(NOT "${${GLUT_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${GLUT_name}_INCLUDE_DIRS})
  endif()
endif()

if(OPENGL_FOUND)
  set(OpenGL_name OPENGL)
  set(OpenGL_FOUND TRUE)
elseif(OpenGL_FOUND)
  set(OpenGL_name OpenGL)
  set(OPENGL_FOUND TRUE)
endif()
if(OpenGL_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_OPENGL)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OpenGL")
  link_directories(${${OpenGL_name}_LIBRARY_DIRS})
  if(NOT "${${OpenGL_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${OpenGL_name}_INCLUDE_DIRS})
  endif()
endif()

if(TUIO_FOUND)
  set(TUIO_name TUIO)
  set(TUIO_FOUND TRUE)
elseif(TUIO_FOUND)
  set(TUIO_name TUIO)
  set(TUIO_FOUND TRUE)
endif()
if(TUIO_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_TUIO)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} TUIO")
  link_directories(${${TUIO_name}_LIBRARY_DIRS})
  if(NOT "${${TUIO_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(SYSTEM ${${TUIO_name}_INCLUDE_DIRS})
  endif()
endif()

if(FFMPEG_FOUND)
  set(FFMPEG_name FFMPEG)
  set(FFMPEG_FOUND TRUE)
elseif(FFMPEG_FOUND)
  set(FFMPEG_name FFMPEG)
  set(FFMPEG_FOUND TRUE)
endif()
if(FFMPEG_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_FFMPEG)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} FFMPEG")
  link_directories(${${FFMPEG_name}_LIBRARY_DIRS})
  if(NOT "${${FFMPEG_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${FFMPEG_name}_INCLUDE_DIRS})
  endif()
endif()

if(OPENMP_FOUND)
  set(OpenMP_name OPENMP)
  set(OpenMP_FOUND TRUE)
elseif(OpenMP_FOUND)
  set(OpenMP_name OpenMP)
  set(OPENMP_FOUND TRUE)
endif()
if(OpenMP_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_OPENMP)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OpenMP")
  link_directories(${${OpenMP_name}_LIBRARY_DIRS})
  if(NOT "${${OpenMP_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${OpenMP_name}_INCLUDE_DIRS})
  endif()
endif()

if(FCGI_FOUND)
  set(FCGI_name FCGI)
  set(FCGI_FOUND TRUE)
elseif(FCGI_FOUND)
  set(FCGI_name FCGI)
  set(FCGI_FOUND TRUE)
endif()
if(FCGI_name)
  list(APPEND FIND_PACKAGES_DEFINES DISPLAYCLUSTER_USE_FCGI)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} FCGI")
  link_directories(${${FCGI_name}_LIBRARY_DIRS})
  if(NOT "${${FCGI_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${FCGI_name}_INCLUDE_DIRS})
  endif()
endif()

set(DISPLAYCLUSTER_BUILD_DEBS autoconf;automake;cmake;doxygen;freeglut3-dev;git;git-review;libavcodec-dev;libavformat-dev;libavutil-dev;libboost-date-time-dev;libboost-program-options-dev;libboost-regex-dev;libboost-serialization-dev;libboost-test-dev;libfcgi-dev;libjpeg-turbo8-dev;libopenmpi-dev;libpoppler-dev;libswscale-dev;libturbojpeg;libxmu-dev;openmpi-bin;pkg-config;subversion)

set(DISPLAYCLUSTER_DEPENDS Boost;LibJpegTurbo;Qt4;MPI;Poppler;GLUT;OpenGL;TUIO;FFMPEG;OpenMP;FCGI)

# Write defines.h and options.cmake
if(NOT PROJECT_INCLUDE_NAME)
  message(WARNING "PROJECT_INCLUDE_NAME not set, old or missing Common.cmake?")
  set(PROJECT_INCLUDE_NAME ${CMAKE_PROJECT_NAME})
endif()
if(NOT OPTIONS_CMAKE)
  set(OPTIONS_CMAKE ${CMAKE_BINARY_DIR}/options.cmake)
endif()
set(DEFINES_FILE "${CMAKE_BINARY_DIR}/include/${PROJECT_INCLUDE_NAME}/defines${SYSTEM}.h")
list(APPEND COMMON_INCLUDES ${DEFINES_FILE})
set(DEFINES_FILE_IN ${DEFINES_FILE}.in)
file(WRITE ${DEFINES_FILE_IN}
  "// generated by CMake/FindPackages.cmake, do not edit.\n\n"
  "#ifndef ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n"
  "#define ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n\n")
file(WRITE ${OPTIONS_CMAKE} "# Optional modules enabled during build\n")
foreach(DEF ${FIND_PACKAGES_DEFINES})
  add_definitions(-D${DEF}=1)
  file(APPEND ${DEFINES_FILE_IN}
  "#ifndef ${DEF}\n"
  "#  define ${DEF} 1\n"
  "#endif\n")
if(NOT DEF STREQUAL SYSTEM)
  file(APPEND ${OPTIONS_CMAKE} "set(${DEF} ON)\n")
endif()
endforeach()
if(CMAKE_MODULE_INSTALL_PATH)
  install(FILES ${OPTIONS_CMAKE} DESTINATION ${CMAKE_MODULE_INSTALL_PATH}
    COMPONENT dev)
else()
  message(WARNING "CMAKE_MODULE_INSTALL_PATH not set, old or missing Common.cmake?")
endif()
file(APPEND ${DEFINES_FILE_IN}
  "\n#endif\n")

include(UpdateFile)
configure_file(${DEFINES_FILE_IN} ${DEFINES_FILE} COPYONLY)
if(Boost_FOUND) # another WAR for broken boost stuff...
  set(Boost_VERSION ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION})
endif()
if(CUDA_FOUND)
  string(REPLACE "-std=c++11" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
  string(REPLACE "-std=c++0x" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
endif()
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
if(QT4_FOUND)
  if(WIN32)
    set(QT_USE_QTMAIN TRUE)
  endif()
  # Configure a copy of the 'UseQt4.cmake' system file.
  if(NOT EXISTS ${QT_USE_FILE})
    message(WARNING "Can't find QT_USE_FILE!")
  else()
    set(_customUseQt4File "${CMAKE_BINARY_DIR}/UseQt4.cmake")
    file(READ ${QT_USE_FILE} content)
    # Change all include_directories() to use the SYSTEM option
    string(REPLACE "include_directories(" "include_directories(SYSTEM " content ${content})
    string(REPLACE "INCLUDE_DIRECTORIES(" "INCLUDE_DIRECTORIES(SYSTEM " content ${content})
    file(WRITE ${_customUseQt4File} ${content})
    set(QT_USE_FILE ${_customUseQt4File})
    include(${QT_USE_FILE})
  endif()
endif()
if(FIND_PACKAGES_FOUND)
  if(MSVC)
    message(STATUS "Configured with ${FIND_PACKAGES_FOUND}")
  else()
    message(STATUS "Configured with ${CMAKE_BUILD_TYPE}${FIND_PACKAGES_FOUND}")
  endif()
endif()
