project(dagr_system)

include(CMake/dagr_system_platform_test.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:dagr_core,INTERFACE_INCLUDE_DIRECTORIES>
    )

set(dagr_system_srcs
    dagr_system_interface.cxx
    )

set(dagr_system_link dagr_core)

if (UNIX)
    include(CheckIncludeFileCXX)
    # check for simple stack trace
    # usually it's in libc but on FreeBSD
    # it's in libexecinfo
    find_library(EXECINFO_LIB "execinfo")
    mark_as_advanced(EXECINFO_LIB)
    if (NOT EXECINFO_LIB)
        set(EXECINFO_LIB "")
    endif()
    CHECK_INCLUDE_FILE_CXX("execinfo.h" DAGR_CXX_HAS_EXECINFOH)
    if (DAGR_CXX_HAS_EXECINFOH)
        # we have the backtrace header check if it
        # can be used  with this compiler
        set(DAGR_PLATFORM_CXX_TEST_LINK_LIBRARIES ${EXECINFO_LIB})
        DAGR_PLATFORM_CXX_TEST(DAGR_CXX_HAS_BACKTRACE
            "Checking whether backtrace works with this C++ compiler"
            DIRECT)
        set(DAGR_PLATFORM_CXX_TEST_LINK_LIBRARIES)
        if (DAGR_CXX_HAS_BACKTRACE)
            # backtrace is supported by this system and compiler.
            # now check for the more advanced capabilities.
            SET_PROPERTY(SOURCE dagr_system_interface.cxx APPEND PROPERTY
                COMPILE_DEFINITIONS DAGR_HAS_BACKTRACE=1)
            # check for symbol lookup using dladdr
            CHECK_INCLUDE_FILE_CXX("dlfcn.h" DAGR_CXX_HAS_DLFCNH)
            if (DAGR_CXX_HAS_DLFCNH)
                # we have symbol lookup libraries and headers
                # check if they can be used with this compiler
                set(DAGR_PLATFORM_CXX_TEST_LINK_LIBRARIES ${CMAKE_DL_LIBS})
                    DAGR_PLATFORM_CXX_TEST(DAGR_CXX_HAS_DLADDR
                    "Checking whether dladdr works with this C++ compiler"
                    DIRECT)
                set(DAGR_PLATFORM_CXX_TEST_LINK_LIBRARIES)
                if (DAGR_CXX_HAS_DLADDR)
                    # symbol lookup is supported by this system
                    # and compiler.
                    SET_PROPERTY(SOURCE dagr_system_interface.cxx APPEND
                        PROPERTY COMPILE_DEFINITIONS DAGR_HAS_SYMBOL_LOOKUP=1)
                endif()
          endif()
          # c++ demangling support
          # check for cxxabi headers
          CHECK_INCLUDE_FILE_CXX("cxxabi.h" DAGR_CXX_HAS_CXXABIH)
          if (DAGR_CXX_HAS_CXXABIH)
              # check if cxxabi can be used with this
              # system and compiler.
              DAGR_PLATFORM_CXX_TEST(DAGR_CXX_HAS_CXXABI
                  "Checking whether cxxabi works with this C++ compiler"
                  DIRECT)
              if (DAGR_CXX_HAS_CXXABI)
                  # c++ demangle using cxxabi is supported with
                  # this system and compiler
                  SET_PROPERTY(SOURCE dagr_system_interface.cxx APPEND
                    PROPERTY COMPILE_DEFINITIONS DAGR_HAS_CPP_DEMANGLE=1)
              endif()
          endif()
          # basic backtrace works better with release build
          # don't bother with advanced features for release
          SET_PROPERTY(SOURCE dagr_system_interface.cxx APPEND
            PROPERTY COMPILE_DEFINITIONS_DEBUG DAGR_HAS_DEBUG_BUILD=1)
          SET_PROPERTY(SOURCE dagr_system_interface.cxx APPEND
            PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO DAGR_HAS_DEBUG_BUILD=1)
        endif()
    endif()
endif()

if (WIN32)
    list(APPEND dagr_system_link ws2_32)
    if (DAGR_SYS_HAS_PSAPI)
        list(APPEND dagr_system_link Psapi)
    endif()
elseif (UNIX)
    if (EXECINFO_LIB AND DAGR_CXX_HAS_BACKTRACE)
        # backtrace on FreeBSD is not in libc
        list(APPEND dagr_system_link ${EXECINFO_LIB})
    endif()
    if (DAGR_CXX_HAS_DLADDR)
        # for symbol lookup using dladdr
        list(APPEND dagr_system_link ${CMAKE_DL_LIBS})
    endif()
endif()

add_library(dagr_system ${dagr_system_srcs})
target_link_libraries(dagr_system ${dagr_system_link})

target_include_directories(dagr_system
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

install(TARGETS dagr_system
    EXPORT dagr_system
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )

install(EXPORT dagr_system
    DESTINATION lib
    FILE dagr_system.cmake
    EXPORT_LINK_INTERFACE_LIBRARIES
    )
