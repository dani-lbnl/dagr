project(dagr_io)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:dagr_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:dagr_data,INTERFACE_INCLUDE_DIRECTORIES>
    )

set(dagr_io_srcs
    dagr_file_util.cxx
    dagr_table_reader.cxx
    dagr_table_writer.cxx
    dagr_vtk_util.cxx
    )

if (WIN32)
    list(APPEND dagr_io_srcs win_windirent.cxx)
endif()

set(dagr_io_link)

if (DAGR_HAS_MPI)
    include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})
    list(APPEND dagr_io_link ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
endif()

if (DAGR_HAS_VTK)
    include(${VTK_USE_FILE})
elseif (DAGR_HAS_PARAVIEW)
    include(${PARAVIEW_USE_FILE})
endif()
list(APPEND dagr_io_srcs dagr_vtk_cartesian_mesh_writer.cxx)
if (DAGR_HAS_VTK OR DAGR_HAS_PARAVIEW)
    list(APPEND dagr_io_link ${VTK_LIBRARIES})
endif()

if (DAGR_HAS_BOOST)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    list(APPEND dagr_io_link ${Boost_LIBRARIES})
endif()

if (DAGR_HAS_LIBXLSXWRITER)
    include_directories(SYSTEM ${LIBXLSXWRITER_INCLUDE_DIR})
    list(APPEND dagr_io_link ${LIBXLSXWRITER_LIBRARIES})
endif()

add_library(dagr_io ${dagr_io_srcs})
target_link_libraries(dagr_io dagr_core dagr_data ${dagr_io_link})

target_include_directories(dagr_io
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

install(TARGETS dagr_io
    EXPORT dagr_io
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )

install(EXPORT dagr_io
    DESTINATION lib
    FILE dagr_io.cmake
    EXPORT_LINK_INTERFACE_LIBRARIES
    )
