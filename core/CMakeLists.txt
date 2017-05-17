project(dagr_core)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(dagr_core_srcs
    dagr_algorithm.cxx
    dagr_algorithm_executive.cxx
    dagr_binary_stream.cxx
    dagr_calendar.cxx
    dagr_dataset.cxx
    dagr_metadata.cxx
    dagr_mpi_manager.cxx
    dagr_parallel_id.cxx
    dagr_temporal_reduction.cxx
    dagr_threaded_algorithm.cxx
    dagr_time_step_executive.cxx
    dagr_variant_array.cxx
    )

set(dagr_core_link)

if (DAGR_HAS_MPI)
    include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})
    list(APPEND dagr_core_link ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
endif()

if (DAGR_HAS_BOOST)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    list(APPEND dagr_core_link ${Boost_LIBRARIES})
endif()

if (DAGR_HAS_UDUNITS)
    include_directories(SYSTEM ${UDUNITS_INCLUDE_DIR})
    list(APPEND dagr_core_link ${UDUNITS_LIBRARY})
    list(APPEND dagr_core_srcs calcalcs.cxx)
endif()

list(APPEND dagr_core_link pthread)

add_library(dagr_core ${dagr_core_srcs})
target_link_libraries(dagr_core ${dagr_core_link})

target_include_directories(dagr_core
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

install(TARGETS dagr_core
    EXPORT dagr_core
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )

install(EXPORT dagr_core
    DESTINATION lib
    FILE dagr_core.cmake
    EXPORT_LINK_INTERFACE_LIBRARIES
    )
