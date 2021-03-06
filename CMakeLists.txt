cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0028 NEW)

project(dagr C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")

# if we forgot to set the build type default to release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release"
      CACHE STRING "One of: Debug Release RelWithDebInfo MinSizeRel"
      FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
        STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Configuring a ${CMAKE_BUILD_TYPE} build")

# add the requisite flags to enable c++11 features
# on various platforms.
if (MSVC)
    # TODO require vs2015
    # enable exceptions
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc"
            CACHE STRING "" FORCE)
else()
    if (NOT CMAKE_CXX_FLAGS)
        set(tmp "-fPIC -std=c++11 -Wall -Wextra")
        if ((APPLE) AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
            set(tmp "${tmp} -stdlib=libc++")
        endif()
        if ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
            set(tmp "${tmp} -march=native -mtune=native")
        endif()
        set(CMAKE_CXX_FLAGS "${tmp}"
            CACHE STRING "DAGR build defaults"
            FORCE)
    endif()
    if (NOT CMAKE_Fortran_FLAGS)
        set(tmp "-Wall -Wextra -Wno-conversion")
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(tmp "${tmp} -march=native -mtune=native")
        endif()
        set(CMAKE_Fortran_FLAGS "${tmp}"
            CACHE STRING "DAGR build defaults"
            FORCE)
    endif()
endif()

# platform specific compile time feature detection
message(STATUS "Check for c++ regex support")
try_run(CXX11_REGEX_WORKS CXX11_REGEX_COMPILES
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/dagr_platform_tests.cpp"
    CMAKE_FLAGS ${CMAKE_CXX_FLAGS}
    COMPILE_DEFINITIONS "-DCXX11_REGEX_TEST")
if (NOT (CXX11_REGEX_COMPILES AND CXX11_REGEX_WORKS))
    message(STATUS "Check for c++ regex support -- not found. Use a c++11 compiler to enable.")
else()
    message(STATUS "Check for c++ regex support -- enabled")
    set(DAGR_HAS_REGEX ON CACHE BOOL "Regex features")
endif()

# locate dependencies
# configure for MPI
set(tmp OFF)
find_package(MPI)
if (MPI_C_FOUND  AND ((DEFINED DAGR_HAS_MPI AND DAGR_HAS_MPI)
    OR (NOT DEFINED DAGR_HAS_MPI)))
    message(STATUS "MPI features -- enabled")
    set(tmp ON)
else()
    message(STATUS "MPI features -- not found.")
endif()
set(DAGR_HAS_MPI ${tmp} CACHE BOOL "MPI features")

set(DAGR_HAS_NETCDF OFF CACHE BOOL "NetCDF features")

# configure for ParaView
set(tmp OFF)
find_package(ParaView QUIET)
if (ParaView_FOUND AND ((DEFINED DAGR_HAS_PARAVIEW AND DAGR_HAS_PARAVIEW) OR (NOT DEFINED DAGR_HAS_PARAVIEW)))
    message(STATUS "ParaView features -- enabled")
    set(tmp ON)
else()
    message(STATUS "ParaView features -- not found. set ParaView_DIR to enable.")
endif()
set(DAGR_HAS_PARAVIEW ${tmp} CACHE BOOL "ParaView features")

# configure for VTK
set(tmp OFF)
if (NOT DAGR_HAS_PARAVIEW)
    find_package(VTK QUIET)
    if (VTK_FOUND AND ((DEFINED DAGR_HAS_VTK AND DAGR_HAS_VTK) OR (NOT DEFINED DAGR_HAS_VTK)))
        message(STATUS "VTK features -- enabled")
        set(tmp ON)
    else()
        message(STATUS "VTK features -- not found. set VTK_DIR to enable.")
    endif()
endif()
set(DAGR_HAS_VTK ${tmp} CACHE BOOL "VTK features")

#configure for Boost
set(tmp OFF)
find_package(Boost QUIET COMPONENTS program_options)
if (Boost_FOUND AND ((DEFINED DAGR_HAS_BOOST AND DAGR_HAS_BOOST) OR (NOT DEFINED DAGR_HAS_BOOST)))
    message(STATUS "Boost features -- enabled")
    set(tmp ON)
else()
    message(STATUS "Boost fetaures -- not found. set BOOST_ROOT to enable.")
    message(WARNING "Boost is required for command line applications")
endif()
set(DAGR_HAS_BOOST ${tmp} CACHE BOOL "Boost features")

#configure for OpenSSL
set(tmp OFF)
find_package(OpenSSL QUIET)
if (OpenSSL_FOUND AND ((DEFINED DAGR_HAS_OPENSSL AND DAGR_HAS_OPENSSL) OR (NOT DEFINED DAGR_HAS_OPENSSL)))
    message(STATUS "OpenSSL features -- enabled")
    set(tmp ON)
else()
    message(STATUS "OpenSSL fetaures -- not found. set OPENSSL_ROOT_DIR to enable.")
endif()
set(DAGR_HAS_OPENSSL ${tmp} CACHE BOOL "OpenSSL features")

# configure for Python
set(tmp OFF)
find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
    find_program(PYTHON_CONFIG_EXECUTABLE python-config)
    if (NOT PYTHON_CONFIG_EXECUTABLE)
        message(SEND_ERROR "python-config executable is required.")
    endif()
    execute_process(COMMAND ${PYTHON_CONFIG_EXECUTABLE} --prefix
        OUTPUT_VARIABLE python_prefix OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(PYTHON_INCLUDE_DIR ${python_prefix}/include/python2.7)
    if (EXISTS ${python_prefix}/lib/libpython2.7${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(PYTHON_LIBRARY ${python_prefix}/lib/libpython2.7${CMAKE_SHARED_LIBRARY_SUFFIX})
    elseif (EXISTS ${python_prefix}/lib64/libpython2.7${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(PYTHON_LIBRARY ${python_prefix}/lib64/libpython2.7${CMAKE_SHARED_LIBRARY_SUFFIX})
    elseif (EXISTS ${python_prefix}/lib/x86_64-linux-gnu/libpython2.7${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(PYTHON_LIBRARY ${python_prefix}/lib/x86_64-linux-gnu/libpython2.7${CMAKE_SHARED_LIBRARY_SUFFIX})
    else()
        message(SEND_ERROR "Failed to locate Python library for ${python_prefix}")
    endif()
endif()
find_package(PythonLibs)
find_package(Numpy)
find_program(swig_cmd NAMES swig swig3.0)
find_package(Mpi4Py)
if ((PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND NUMPY_FOUND AND swig_cmd)
    AND ((DEFINED DAGR_HAS_PYTHON AND DAGR_HAS_PYTHON)
    OR (NOT DEFINED DAGR_HAS_PYTHON)))
    message(STATUS "Python features -- enabled")
    set(tmp ON)
else()
    message(STATUS "Python features -- disabled")
endif()
set(DAGR_HAS_PYTHON ${tmp} CACHE BOOL "Python binding")

# configure library type
set(CMAKE_MACOSX_RPATH 1)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
if (DAGR_HAS_PYTHON)
    # SWIG RTTI system requires libs when multiple modules
    # have cross dependencies
    set(BUILD_SHARED_LIBS ON FORCE)
endif()

# get the build version descriptor
set(tmp)
find_package(Git)
if (GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE}
        --git-dir=${CMAKE_CURRENT_SOURCE_DIR}/.git describe --tags
        OUTPUT_VARIABLE tmp OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
if (NOT tmp)
    set(tmp "0.0.0")
endif()
set(DAGR_VERSION_DESCR ${tmp} CACHE STRING "DAGR version descriptor")
message(STATUS "DAGR version ${DAGR_VERSION_DESCR}")

# generate global configuration
include(CMake/dagr_test.cmake)
include(CMake/dagr_app.cmake)
include(CMake/dagr_python.cmake)
configure_file(dagr_config.h.in dagr_config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# build library components and applications
add_subdirectory(core)
add_subdirectory(data)
add_subdirectory(io)
add_subdirectory(alg)
add_subdirectory(python)
add_subdirectory(system)
#add_subdirectory(paraview)

# enable regression tests
set(BUILD_TESTING OFF CACHE BOOL "Enable tests")
if (BUILD_TESTING)
    include(CTest)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
        @ONLY)
    if (NOT DEFINED DAGR_DATA_ROOT)
       set(tmp "${CMAKE_CURRENT_SOURCE_DIR}/../DAGR_data")
    else()
        set(tmp ${DAGR_DATA_ROOT})
    endif()
    if (EXISTS "${tmp}")
        message(STATUS "Test data -- available")
    else()
        message(STATUS "Test data -- not available")
        set(tmp)
    endif()
    set(DAGR_DATA_ROOT "${tmp}"
        CACHE PATH "Path to DAGR test data directory")
    set(DAGR_TEST_CORES 4 CACHE STRING
        "Number of cores for use in parallel tests")
    math(EXPR HALF_CORES "${DAGR_TEST_CORES}/2")
    if (HALF_CORES LESS 1)
        message(FATAL_ERROR "Parallel test require at lest 2 cores")
    endif()
    add_subdirectory(test)
endif()
