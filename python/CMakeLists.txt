project(dagr_py)

if (DAGR_HAS_PYTHON)
    include(CMake/dagr_python.cmake)

    depend_swig(dagr_py.i dagr_py.dep)
    wrap_swig(dagr_py.i dagr_py.cxx dagr_py.dep)

    include_directories(SYSTEM ${PYTHON_INCLUDE_PATH} ${NUMPY_INCLUDE_DIR})
    include_directories(${CMAKE_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/core
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_SOURCE_DIR}/io
            ${CMAKE_SOURCE_DIR}/alg
            ${CMAKE_SOURCE_DIR}/system
            )

    PYTHON_ADD_MODULE(_dagr_py ${CMAKE_CURRENT_BINARY_DIR}/dagr_py.cxx)

    target_link_libraries(_dagr_py ${PYTHON_LIBRARIES}
        dagr_core dagr_data dagr_alg dagr_io dagr_system)

    add_custom_command(TARGET _dagr_py POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/dagr_py.py
            ${CMAKE_CURRENT_BINARY_DIR}/../lib)

    configure_file(dagr.py ${CMAKE_CURRENT_BINARY_DIR}/../lib/dagr.py
        COPYONLY)

    install(TARGETS _dagr_py LIBRARY DESTINATION lib)
    install(FILES dagr.py ${CMAKE_CURRENT_BINARY_DIR}/dagr_py.py
        DESTINATION lib)
endif()
