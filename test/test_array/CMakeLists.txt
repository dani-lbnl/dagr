project(dagr_test_array)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:dagr_core,INTERFACE_INCLUDE_DIRECTORIES>
    )

set(array_test_srcs
    array.cxx
    array_add.cxx
    array_executive.cxx
    array_scalar_multiply.cxx
    array_source.cxx
    array_temporal_stats.cxx
    array_time_average.cxx
    array_writer.cxx
    )

add_library(dagr_test_array ${array_test_srcs})
target_link_libraries(dagr_test_array dagr_core)

target_include_directories(dagr_test_array
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    )
