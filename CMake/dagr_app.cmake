# dagr_add_app(name
#   SOURCES  -- optional, source files to comile
#   LIBS -- optional, libraries to link to the compiled test
#   FEATURES -- optional, boolean condition decribing feature dependencies
#   )
function (dagr_add_app app_name)
    set(opt_args)
    set(val_args FEATURES)
    set(array_args SOURCES LIBS)
    cmake_parse_arguments(APP
        "${opt_args}" "${val_args}" "${array_args}" ${ARGN})
    if (APP_FEATURES)
        if (NOT APP_SOURCES)
            set(APP_SOURCES "${app_name}.cpp")
        endif()
        add_executable(${app_name} ${APP_SOURCES})
        if (APP_LIBS)
            target_link_libraries(${app_name}
                dagr_core dagr_data dagr_io dagr_alg
                ${APP_LIBS})
        endif()
        install(TARGETS ${app_name} RUNTIME DESTINATION bin)
    endif()
endfunction()
