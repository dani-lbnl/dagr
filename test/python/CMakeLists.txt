# TODO -- camke_parse_arguments eats ""
if (DAGR_DATA_ROOT AND DAGR_HAS_NETCDF)
    add_test(NAME py_test_connected_components
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_connected_components.py
        "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-01-10800\\.nc"
        "" U850 V850 15 0 0 "py_test_connected_components_%t%.vtk"
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

dagr_add_test(py_test_derived_quantity
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_derived_quantity.py
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-0[12]-10800\\.nc"
    U850 V850 0 0 "py_test_derived_quantity_%t%.vtk"
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(py_test_programmable_algorithm
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_programmable_algorithm.py
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-0[12]-10800\\.nc"
    U850 V850 0 0 "py_test_programmable_algorithm_%t%.vtk"
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(py_test_vector_ops
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_vector_ops.py
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-01-10800\\.nc"
    0 0 py_test_vector_ops_%t%.vtk U850 V850
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(py_test_array_collection
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_array_collection.py)

dagr_add_test(py_test_metadata
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_metadata.py)

dagr_add_test(py_test_variant_array
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_variant_array.py)

dagr_add_test(py_test_database
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_database.py)

dagr_add_test(py_test_programmable_map_reduce_serial
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_programmable_map_reduce.py
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-0[12]-10800\\.nc"
    "${DAGR_DATA_ROOT}/py_test_programmable_map_reduce.bin" 0 -1 1 TMQ T200 T500
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(py_test_programmable_map_reduce_thread
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_programmable_map_reduce.py
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-0[12]-10800\\.nc"
    "${DAGR_DATA_ROOT}/py_test_programmable_map_reduce.bin" 0 -1 4 TMQ T200 T500
    FEATURES ${DAGR_HAS_NETCDF}
    REQ_DAGR_DATA)

dagr_add_test(py_test_programmable_map_reduce_mpi
    COMMAND ${MPIEXEC} -n ${DAGR_TEST_CORES} python
    ${CMAKE_CURRENT_SOURCE_DIR}/test_programmable_map_reduce.py
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-0[12]-10800\\.nc"
    "${DAGR_DATA_ROOT}/py_test_programmable_map_reduce.bin" 0 -1 1 TMQ T200 T500
    FEATURES (${DAGR_HAS_NETCDF} AND ${DAGR_HAS_MPI} AND ${MPI4PY_FOUND})
    REQ_DAGR_DATA)

dagr_add_test(py_test_programmable_map_reduce_mpi_thread
    COMMAND ${MPIEXEC} -n ${HALF_CORES} python
    ${CMAKE_CURRENT_SOURCE_DIR}/test_programmable_map_reduce.py
    "${DAGR_DATA_ROOT}/cam5_1_amip_run2\\.cam2\\.h2\\.1991-10-0[12]-10800\\.nc"
    "${DAGR_DATA_ROOT}/py_test_programmable_map_reduce.bin" 0 -1 ${HALF_CORES}
    TMQ T200 T500
    FEATURES (${DAGR_HAS_NETCDF} AND ${DAGR_HAS_MPI} AND ${MPI4PY_FOUND})
    REQ_DAGR_DATA)

dagr_add_test(py_test_event_filter
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/test_event_filter.py
    "${DAGR_DATA_ROOT}/test_tc_candidates_20.bin"
    "${DAGR_DATA_ROOT}/test_event_filter.bin"
    REQ_DAGR_DATA)
