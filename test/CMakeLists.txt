project(dagr_test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/test)

add_subdirectory(test_array)
if (DAGR_HAS_PYTHON)
    add_subdirectory(python)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:dagr_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_data,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_alg,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_io,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_system,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_test_array,INTERFACE_INCLUDE_DIRECTORIES>)

set(dagr_test_link pthread dagr_system)

if (DAGR_HAS_MPI)
    include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})
    list(APPEND dagr_test_link ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
endif()

if (DAGR_HAS_BOOST)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    list(APPEND dagr_test_link ${Boost_LIBRARIES})
endif()

dagr_add_test(test_pipeline_time_average
    SOURCES test_pipeline_time_average.cpp
    LIBS dagr_core dagr_test_array ${dagr_test_link}
    COMMAND test_pipeline_time_average)

dagr_add_test(test_pipeline_branches
    SOURCES test_pipeline_branches.cpp
    LIBS dagr_core dagr_test_array ${dagr_test_link}
    COMMAND test_pipeline_branches)

dagr_add_test(test_pipeline_temporal_reduction
    SOURCES test_pipeline_temporal_reduction.cpp
    LIBS dagr_core dagr_test_array ${dagr_test_link}
    COMMAND test_pipeline_temporal_reduction)

dagr_add_test(test_stack_trace_signal_handler
    SOURCES test_stack_trace_signal_handler.cpp
    LIBS ${dagr_test_link}
    COMMAND test_stack_trace_signal_handler 8 0)

dagr_add_test(test_connected_components
    SOURCES test_connected_components.cpp
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_connected_components
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-0[12]-10800\\.nc"
    lon lat "." time U850 V850 0 0 15 "test_connected_components_%t%.%e%"
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(test_cartesian_mesh_regrid
    SOURCES test_cartesian_mesh_regrid.cpp
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_cartesian_mesh_regrid
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-01-10800\\.nc"
    lon lat "." time 1 PSL "${DAGR_DATA_ROOT}/landsea\\.nc" lon lat "." "." 1 LSMASK
    test_cartesian_mesh_regrid.%e% 0 0 1 359 -89 89 0 0
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(test_temporal_average
    SOURCES test_temporal_average.cpp
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_temporal_average
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2.cam2.h2.1991-10-01-10800.nc"
    test_temporal_average_%t%.%e% 0 -1 3 U850
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(test_table_writer
    SOURCES test_table_writer.cpp dagr_test_util.cxx
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_table_writer)

dagr_add_test(test_table_reader
    SOURCES test_table_reader.cpp dagr_test_util.cxx
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_table_reader)

dagr_add_test(test_dataset_diff
    SOURCES test_dataset_diff.cpp dagr_test_util.cxx
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_dataset_diff)

dagr_add_test(test_table_sort
    SOURCES test_table_sort.cpp
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_table_sort "${DAGR_DATA_ROOT}/cam5_temperature_stats.bin"
    "${DAGR_DATA_ROOT}/test_table_sort.bin" step
    REQ_DAGR_DATA)

dagr_add_test(test_descriptive_statistics_serial
    EXEC_NAME test_descriptive_statistics
    SOURCES test_descriptive_statistics.cpp dagr_test_util.cxx
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_descriptive_statistics
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-[0-9][0-9]-10800\\.nc"
    "${DAGR_DATA_ROOT}/test_descriptive_statistics.bin" 0 -1 1 TMQ T200 T500
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(test_descriptive_statistics_threads
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_descriptive_statistics
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-[0-9][0-9]-10800\\.nc"
    "${DAGR_DATA_ROOT}/test_descriptive_statistics.bin" 0 -1 ${DAGR_TEST_CORES}
    TMQ T200 T500
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(test_descriptive_statistics_mpi
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND ${MPIEXEC} -n ${DAGR_TEST_CORES} test_descriptive_statistics
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-[0-9][0-9]-10800\\.nc"
    "${DAGR_DATA_ROOT}/test_descriptive_statistics.bin" 0 -1 1 TMQ T200 T500
    FEATURES (${DAGR_HAS_NETCDF} AND ${DAGR_HAS_MPI})
    REQ_DAGR_DATA)

dagr_add_test(test_descriptive_statistics_mpi_threads
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND ${MPIEXEC} -n ${HALF_CORES} test_descriptive_statistics
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-[0-9][0-9]-10800\\.nc"
    "${DAGR_DATA_ROOT}/test_descriptive_statistics.bin" 0 -1 ${HALF_CORES}
    TMQ T200 T500
    FEATURES (${DAGR_HAS_NETCDF} AND ${DAGR_HAS_MPI})
    REQ_DAGR_DATA)

dagr_add_test(test_binary_stream
    SOURCES test_binary_stream.cpp
    LIBS dagr_core dagr_data dagr_alg ${dagr_test_link}
    COMMAND test_binary_stream)

dagr_add_test(test_binary_stream_mpi
    SOURCES test_binary_stream.cpp
    LIBS dagr_core dagr_data dagr_alg ${dagr_test_link}
    COMMAND ${MPIEXEC} -n 2 test_binary_stream)

dagr_add_test(test_table_reader_distribute_serial
    COMMAND test_table_reader_distribute
    "${DAGR_DATA_ROOT}/test_tc_candidates_20.bin"
    "${DAGR_DATA_ROOT}/test_table_reader_distribute_20.bin"
    "step" 0 -1 1
    REQ_DAGR_DATA)

dagr_add_test(test_table_reader_distribute_mpi
    COMMAND ${MPIEXEC} -n ${DAGR_TEST_CORES} test_table_reader_distribute
    "${DAGR_DATA_ROOT}/test_tc_candidates_20.bin"
    "${DAGR_DATA_ROOT}/test_table_reader_distribute_20.bin"
    "step" 0 -1 1
    FEATURES ${DAGR_HAS_MPI}
    REQ_DAGR_DATA)

dagr_add_test(test_table_reader_distribute_threads
    EXEC_NAME test_table_reader_distribute
    SOURCES test_table_reader_distribute.cpp dagr_test_util.cxx
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_table_reader_distribute
    "${DAGR_DATA_ROOT}/test_tc_candidates_20.bin"
    "${DAGR_DATA_ROOT}/test_table_reader_distribute_20.bin"
    "step" 0 -1 ${DAGR_TEST_CORES}
    REQ_DAGR_DATA)

dagr_add_test(test_table_reader_distribute_mpi_threads
    COMMAND ${MPIEXEC} -n ${HALF_CORES} test_table_reader_distribute
    "${DAGR_DATA_ROOT}/test_tc_candidates_20.bin"
    "${DAGR_DATA_ROOT}/test_table_reader_distribute_20.bin"
    "step" 0 -1 ${HALF_CORES}
    FEATURES ${DAGR_HAS_MPI}
    REQ_DAGR_DATA)

dagr_add_test(test_type_select
    SOURCES test_type_select.cpp
    LIBS dagr_core dagr_alg ${dagr_test_link}
    COMMAND test_type_select)

dagr_add_test(test_parser
    SOURCES test_parser.cpp
    LIBS dagr_core dagr_alg ${dagr_test_link}
    COMMAND test_parser)

dagr_add_test(test_variant_array_operator
    SOURCES test_variant_array_operator.cpp
    LIBS dagr_core dagr_alg ${dagr_test_link}
    COMMAND test_variant_array_operator)

dagr_add_test(test_evaluate_expression_table
    SOURCES test_evaluate_expression_table.cpp
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_evaluate_expression_table "${DAGR_DATA_ROOT}/tracks_1990s_3hr_mdd_4800.bin"
    "${DAGR_DATA_ROOT}/test_evaluate_expression_table.bin"
    "!((track_id==191)&&((surface_wind*3.6d>=209.0d)&&(surface_wind*3.6d<249.0d)))"
    REQ_DAGR_DATA)

dagr_add_test(test_table_remove_rows_track_id
    EXEC_NAME test_table_remove_rows
    SOURCES test_table_remove_rows.cpp
    LIBS dagr_core dagr_data dagr_io dagr_alg ${dagr_test_link}
    COMMAND test_table_remove_rows "${DAGR_DATA_ROOT}/tracks_1990s_3hr_mdd_4800.bin"
    "${DAGR_DATA_ROOT}/tracks_1990s_3hr_mdd_4800_median_in_cat.bin"
    "(track_id==488)||(track_id==186)||(track_id==578)||(track_id==4)||(track_id==523)||(track_id==191)"
    REQ_DAGR_DATA)

dagr_add_test(test_table_remove_rows_cat_4
    COMMAND test_table_remove_rows "${DAGR_DATA_ROOT}/tracks_1990s_3hr_mdd_4800.bin"
    "${DAGR_DATA_ROOT}/tracks_1990s_3hr_mdd_4800_cat_4.bin"
    "!((surface_wind*3.6d>=209.0d)&&(surface_wind*3.6d<249.0d))"
    REQ_DAGR_DATA)
