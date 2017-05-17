project(dagr_alg)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:dagr_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_data,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_io,INTERFACE_INCLUDE_DIRECTORIES>
    )

set(dagr_alg_cxx_srcs
    dagr_apply_binary_mask.cxx
    dagr_binary_segmentation.cxx
    dagr_cartesian_mesh_subset.cxx
    dagr_cartesian_mesh_regrid.cxx
    dagr_connected_components.cxx
    dagr_dataset_capture.cxx
    dagr_dataset_source.cxx
    dagr_derived_quantity.cxx
    dagr_descriptive_statistics.cxx
    dagr_evaluate_expression.cxx
    dagr_l2_norm.cxx
    dagr_mask.cxx
    dagr_parser.cxx
    dagr_programmable_algorithm.cxx
    dagr_programmable_reduce.cxx
    dagr_table_reduce.cxx
    dagr_table_remove_rows.cxx
    dagr_table_sort.cxx
    dagr_table_to_stream.cxx
    dagr_temporal_average.cxx
    dagr_variant_array_operand.cxx
    dagr_vorticity.cxx
    dagr_dataset_diff.cxx
    )

set(dagr_alg_link pthread)

if (DAGR_HAS_MPI)
    include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})
    list(APPEND dagr_alg_link ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
endif()

if (DAGR_HAS_BOOST)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    list(APPEND dagr_alg_link ${Boost_LIBRARIES})
endif()

add_library(dagr_alg ${dagr_alg_cxx_srcs} ${dagr_alg_f90_srcs})
target_link_libraries(dagr_alg dagr_io dagr_data dagr_core ${dagr_alg_link})

target_include_directories(dagr_alg
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

install(TARGETS dagr_alg
    EXPORT dagr_alg
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )

install(EXPORT dagr_alg
    DESTINATION lib
    FILE dagr_alg.cmake
    EXPORT_LINK_INTERFACE_LIBRARIES
    )
