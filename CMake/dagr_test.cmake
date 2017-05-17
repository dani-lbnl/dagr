# dagr_add_test(name
#   EXEC_NAME -- optional, name of the copiled test
#   SOURCES  -- optional, source files to comile
#   LIBS -- optional, libraries to link to the compiled test
#   COMMAND -- required, test command
#   FEATURES -- optional, boolean condition decribing feature dependencies
#   REQ_DAGR_DATA -- flag whose presence indicates the test needs the data repo
#   )
function (dagr_add_test T_NAME)
    set(opt_args REQ_DAGR_DATA)
    set(val_args EXEC_NAME FEATURES)
    set(array_args SOURCES LIBS COMMAND)
    cmake_parse_arguments(T "${opt_args}" "${val_args}" "${array_args}" ${ARGN})
    set(TEST_ENABLED OFF)
    if (T_FEATURES)
        set(TEST_ENABLED ON)
    endif()
    if (NOT DEFINED T_FEATURES)
        set(TEST_ENABLED ON)
    endif()
    if (TEST_ENABLED)
        if (T_SOURCES)
            set(EXEC_NAME ${T_NAME})
            if (T_EXEC_NAME)
                set(EXEC_NAME ${T_EXEC_NAME})
            endif()
            add_executable(${EXEC_NAME} ${T_SOURCES})
            if (T_LIBS)
                target_link_libraries(${EXEC_NAME} ${T_LIBS})
            endif()
        endif()
        if ((T_REQ_DAGR_DATA AND DAGR_DATA_ROOT) OR NOT T_REQ_DAGR_DATA)
            add_test(NAME ${T_NAME} COMMAND ${T_COMMAND}
                WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            set_tests_properties(${T_NAME}
                PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;FAIL")
        endif()
    endif()
endfunction()