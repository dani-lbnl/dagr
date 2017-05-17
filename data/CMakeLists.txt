project(dagr_data)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:dagr_core,INTERFACE_INCLUDE_DIRECTORIES>
    )

set(dagr_data_srcs
    dagr_array_collection.cxx
    dagr_cartesian_mesh.cxx
    dagr_coordinate_util.cxx
    dagr_mesh.cxx
    dagr_table.cxx
    dagr_table_collection.cxx
    dagr_uniform_cartesian_mesh.cxx
    dagr_database.cxx
    )

add_library(dagr_data ${dagr_data_srcs})
target_link_libraries(dagr_data dagr_core)

target_include_directories(dagr_data
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

install(TARGETS dagr_data
    EXPORT dagr_data
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )

install(EXPORT dagr_data
    DESTINATION lib
    FILE dagr_data.cmake
    EXPORT_LINK_INTERFACE_LIBRARIES
    )
